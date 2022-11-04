// *** serial.c *******************************************************
// This file implements an interrupt driven serial console.

#include "main.h"

#define BAUDRATE  9600

#define BUFFERSIZE  64

static byte rxbuffer[BUFFERSIZE];
static int rxlength;  // number of rx bytes in buffer

static bool waiting;  // we have sent Ctrl-S at end of command
static bool suspend;  // set this to send Ctrl-S in next send() loop
static bool resume;  // set this to send Ctrl-Q in next send() loop
static bool busy;  // we're in the middle of a send() loop

#define CTRLS  ('S'-'@')  // suspend
#define CTRLQ  ('Q'-'@')  // resume

bool serial_active;

int serial_baudrate;

#if SODEBUG
volatile bool serial_in_isr;
volatile int32 serial_in_ticks;
volatile int32 serial_out_ticks;
volatile int32 serial_max_ticks;
#endif

// this function disables the serial isr when BASIC takes over control
void
serial_disable(void)
{
    serial_active = false;

#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    // disable uart0 receive interrupts
    MCF_UART0_UIMR = 0;
#elif MCF51JM128 || MCF51QE128 || MC9S08QE128 || MC9S12DT256 || MC9S12DP512
    // disable uart1 receive interrupts
    SCI1C2X &= ~SCI1C2_RIE_MASK;
#elif MCF51CN128
    // disable uart2 receive interrupts
    SCI2C2X &= ~SCI2C2_RIE_MASK;
#elif PIC32
    // Unconfigure UART2 RX Interrupt
#if SERIAL_UART
#if PIC32PPS
    INTEnable(INT_U2RX, INT_DISABLED);
#else
    ConfigIntUART2(0);
#endif
#else
#if PIC32PPS
    INTEnable(INT_U1RX, INT_DISABLED);
#else
    ConfigIntUART1(0);
#endif
#endif
#endif
    // don't allow the rx ball to start rolling
    waiting = false;
}


// this function acknowledges receipt of a serial command from upper
// level code.
void
serial_command_ack(void)
{
    int x;
    bool boo;
    
    ASSERT(gpl() == 0);

    x = splx(7);
    
    // if we held off the serial rx...
    if (waiting) {
        // receive new characters
        if (rxlength) {    
            // accumulate new commands
            boo = terminal_receive(rxbuffer, rxlength);
            rxlength = 0;
            
            // if a full command was accumulated...
            if (! boo) {
                // wait for another terminal_command_ack();
                goto XXX_SKIP_XXX;
            }
        }
        
        // start the rx ball rolling
        waiting = false;
        
        // if the tx is ready...
        if (pin_uart_tx_ready(SERIAL_UART)) {
            // stuff a resume
            pin_uart_tx(SERIAL_UART, CTRLQ);
        } else {
            // record we need a resume
            suspend = false;
            resume = true;
            assert(busy);
        }
    }
    
XXX_SKIP_XXX:
    splx(x);
}

INTERRUPT
void
#if PIC32
#if defined(__32MX250F128B__)
#if SERIAL_UART
__ISR(37, ipl2) // REVISIT -- ipl?
#else
__ISR(32, ipl2) // REVISIT -- ipl?
#endif
#else
#if SERIAL_UART
__ISR(32, ipl2) // REVISIT -- ipl?
#else
__ISR(24, ipl2) // REVISIT -- ipl?
#endif
#endif
#endif
serial_isr(void)
{
    char c;
    bool boo;
    
    assert(! serial_in_isr);
    assert((serial_in_isr = true) ? true : true);
    assert((serial_in_ticks = ticks) ? true : true);

#if ! MC9S08QE128 && ! MC9S12DT256 && ! MC9S12DP512
    (void)splx(7);
#endif

#if PIC32
    // Clear the RX interrupt Flag
#if SERIAL_UART
#if PIC32PPS
    INTClearFlag(INT_U2RX);
#else
    mU2RXClearIntFlag();
#endif
#else
#if PIC32PPS
    INTClearFlag(INT_U1RX);
#else
    mU1RXClearIntFlag();
#endif
#endif
#endif

    do {
        // process the receive fifo
        while (pin_uart_rx_ready(SERIAL_UART)) {
            c = pin_uart_rx(SERIAL_UART);
            if (c && c != CTRLS && c != CTRLQ) {
                if (c == '\r') {
                    serial_active = true;
                }
                rxbuffer[rxlength++] = c;
                assert(rxlength < sizeof(rxbuffer));
            }
        }
        
        
        // receive characters
        if (rxlength && ! waiting) {    
            // accumulate commands
            boo = terminal_receive(rxbuffer, rxlength);
            rxlength = 0;
            
            // if a full command was accumulated...
            if (! boo) {
                // if the tx is ready...
                if (pin_uart_tx_ready(SERIAL_UART)) {
                    // stuff a suspend
                    pin_uart_tx(SERIAL_UART, CTRLS);
                } else {
                    // record we need a suspend
                    suspend = true;
                    resume = false;
                    assert(busy);
                }
                
                // drop the ball
                waiting = true;
            }
        }
    } while (pin_uart_rx_ready(SERIAL_UART));
    
    assert(serial_in_isr);
    assert((serial_in_isr = false) ? true : true);
    assert((serial_out_ticks = ticks) ? true : true);
    assert((serial_max_ticks = MAX(serial_max_ticks, serial_out_ticks-serial_in_ticks)) ? true : true);
}

void
serial_send(const byte *buffer, int length)
{
    ASSERT(gpl() == 0);

    assert(! busy);
    busy = true;
    
    while (! pin_uart_tx_ready(SERIAL_UART)) {
        // revisit -- poll?
    }
    
    // send the characters synchronously
    for (;;) {
        // if we have a suspend or resume to send, they take precedence
        if (suspend) {
            pin_uart_tx(SERIAL_UART, CTRLS);
            suspend = false;
            assert(! resume);
        } else if (resume) {
            pin_uart_tx(SERIAL_UART, CTRLQ);
            resume = false;
            assert(! suspend);
        } else {
            if (! length) {
                break;
            }
            
            pin_uart_tx(SERIAL_UART, *buffer);
            buffer++;
            length--;
        }
        
        while (! pin_uart_tx_ready(SERIAL_UART)) {
            // revisit -- poll?
        }
    }
    
    assert(busy);
    busy = false;
}

void
serial_initialize(void)
{
    // get the baudrate from flash; revert to 9600 on autorun disable
    serial_baudrate = var_get_flash(FLASH_BAUD);
    if (! serial_baudrate || serial_baudrate == -1 || disable_autorun) {
        serial_baudrate = BAUDRATE;
    }

    // configure the first uart for serial terminal by default
    pin_uart_configure(SERIAL_UART, serial_baudrate, 8, 2, false);
    
    // start us out with a CTRLQ
    pin_uart_tx(SERIAL_UART, CTRLQ);

#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    // UART 0
    // PORT UA (4 pins)
    // UCTS0#, URTS0#, URXD0, and UTXD0 are "primary" functions of their pin group.
    // Set to primary function (0x01).
    MCF_GPIO_PUAPAR = 0x55;
    
    // enable uart0 interrupt.
    MCF_INTC0_ICR13 = MCF_INTC_ICR_IL(SPL_SERIAL)|MCF_INTC_ICR_IP(SPL_SERIAL-2);
    MCF_INTC0_IMRL &= ~MCF_INTC_IMRL_MASKALL;
    MCF_INTC0_IMRL &= ~MCF_INTC_IMRL_INT_MASK13;  // uart0
    
    // configure uart0 receive interrupts
    MCF_UART0_UIMR = MCF_UART_UIMR_FFULL_RXRDY;
#elif MCF51JM128 || MCF51QE128 || MC9S08QE128 || MC9S12DT256 || MC9S12DP512
    SCI1C2X = SCI1C2_TE_MASK|SCI1C2_RE_MASK;
    
    // configure uart1 receive interrupts
    SCI1C2X |= SCI1C2_RIE_MASK;
#elif MCF51CN128
    SCI2C2X = SCI2C2_TE_MASK|SCI2C2_RE_MASK;
    
    // configure uart2 receive interrupts
    SCI2C2X |= SCI2C2_RIE_MASK;
    
    PTDPF2 = 0xa0;
#elif PIC32
#if SERIAL_UART
    U2MODE |= _U2MODE_UARTEN_MASK;

    // Configure UART1 RX Interrupt
#if PIC32PPS
    INTSetVectorPriority(INT_UART_2_VECTOR, INT_PRIORITY_LEVEL_2);
    INTEnable(INT_U2RX, INT_ENABLED);
#else
    ConfigIntUART2(UART_INT_PR2 | UART_RX_INT_EN);
#endif
#else
    U1MODE |= _U1MODE_UARTEN_MASK;

    // Configure UART1 RX Interrupt
#if PIC32PPS
    INTSetVectorPriority(INT_UART_1_VECTOR, INT_PRIORITY_LEVEL_2);
    INTEnable(INT_U1RX, INT_ENABLED);
#else
    ConfigIntUART1(UART_INT_PR2 | UART_RX_INT_EN);
#endif
#endif
#else
#error
#endif
}

