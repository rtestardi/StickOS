// *** sleep.c ********************************************************
// this file implements the low power sleep mode.

#include "main.h"

#if PICTOCRYPT

static volatile int sleep_seconds;

// called on sleep entry and exit (sw1 depressed)
INTERRUPT
void
sleep_isr(void)
{
    (void)splx(7);
    
    delay(100);  // debounce
    while (! (MCF_EPORT_EPPDR & MCF_EPORT_EPPDR_EPPD1)) {
        // NULL
    }
    delay(100);  // debounce

    // this will put us to sleep on the next sleep_poll() or be
    // ignored if we're waking up
    sleep_delay(0);
    
    MCF_EPORT_EPFR = 0x02;
}

void
sleep_delay(int secs)
{
    // potentially put us to sleep on a subsequent sleep_poll()
    sleep_seconds = seconds+secs;
}

void
sleep_delay_off(void)
{
    sleep_seconds = 0;
}

static byte ddrnq, ddran, ddras, ddrqs, ddrta, ddrtc, ddrua, ddrub, ddruc;
static byte panpar, paspar, ptapar, ptcpar, puapar, pubpar, pucpar;
static short pnqpar, pqspar;

// this function puts us to sleep if the sleep switch has been pressed
// and wakes us up when it is pressed again.
void
sleep_poll(void)
{
    bool wake_mode;
    bool sleep_mode;
    
    wake_mode = false;
    sleep_mode = false;
    
    // if we're in auto-sleep mode...
    if (sleep_seconds) {
        // if the time has elapsed...
        if (seconds >= sleep_seconds) {
            sleep_mode = true;
        }
    }

    if (sleep_mode) {
        // save our data directions and pin assignments
        ddrnq = MCF_GPIO_DDRNQ;
        ddran = MCF_GPIO_DDRAN;
        ddras = MCF_GPIO_DDRAS;
        ddrqs = MCF_GPIO_DDRQS;
        ddrta = MCF_GPIO_DDRTA;
        ddrtc = MCF_GPIO_DDRTC;
        ddrua = MCF_GPIO_DDRUA;
        ddrub = MCF_GPIO_DDRUB;
        ddruc = MCF_GPIO_DDRUC;

        pnqpar = MCF_GPIO_PNQPAR;
        panpar = MCF_GPIO_PANPAR;
        paspar = MCF_GPIO_PASPAR;
        pqspar = MCF_GPIO_PQSPAR;
        ptapar = MCF_GPIO_PTAPAR;
        ptcpar = MCF_GPIO_PTCPAR;
        puapar = MCF_GPIO_PUAPAR;
        pubpar = MCF_GPIO_PUBPAR;
        pucpar = MCF_GPIO_PUCPAR;

        // configure all pins for digital input, except IRQ1*
        MCF_GPIO_DDRNQ = 0;
        MCF_GPIO_DDRAN = 0;
        MCF_GPIO_DDRAS = 0;
        MCF_GPIO_DDRQS = 0;
        MCF_GPIO_DDRTA = 0;
        MCF_GPIO_DDRTC = 0;
#if ! DEMO
        MCF_GPIO_DDRUA = 0;
#endif
        MCF_GPIO_DDRUB = 0;
        MCF_GPIO_DDRUC = 0;

        MCF_GPIO_PNQPAR = 0x04;
        MCF_GPIO_PANPAR = 0;
        MCF_GPIO_PASPAR = 0;
        MCF_GPIO_PQSPAR = 0;
        MCF_GPIO_PTAPAR = 0;
        MCF_GPIO_PTCPAR = 0;
#if ! DEMO
        MCF_GPIO_PUAPAR = 0;
#endif
        MCF_GPIO_PUBPAR = 0;
        MCF_GPIO_PUCPAR = 0;

        // power off the adc
        adc_sleep();

        // prepare for stop mode
        MCF_PMM_LPCR = MCF_PMM_LPCR_LPMD_STOP|0x18/*all clocks off*/;
        if (usb_host_mode && other_attached) {
            // prepare for doze mode to keep USB SOF's alive for other devices
            MCF_PMM_LPCR = MCF_PMM_LPCR_LPMD_DOZE|0x18/*all clocks off*/;
        }
            
        // allow us to wake from stop mode
        MCF_PMM_LPICR = MCF_PMM_LPICR_ENBSTOP|MCF_PMM_LPICR_XLPM_IPL(0);

        if (! gpl()) {
            // stop!
            asm_stop_2000();
        } else {
            // stop!
            asm_stop_2700();
        }
        
        wake_mode = true;
        sleep_seconds = 0;
    }

    if (wake_mode) {
        // restore our data directions and pin assignments
        MCF_GPIO_DDRNQ = ddrnq;
        MCF_GPIO_DDRAN = ddran;
        MCF_GPIO_DDRAS = ddras;
        MCF_GPIO_DDRQS = ddrqs;
        MCF_GPIO_DDRTA = ddrta;
        MCF_GPIO_DDRTC = ddrtc;
#if ! DEMO
        MCF_GPIO_DDRUA = ddrua;
#endif
        MCF_GPIO_DDRUB = ddrub;
        MCF_GPIO_DDRUC = ddruc;

        MCF_GPIO_PNQPAR = pnqpar;
        MCF_GPIO_PANPAR = panpar;
        MCF_GPIO_PASPAR = paspar;
        MCF_GPIO_PQSPAR = pqspar;
        MCF_GPIO_PTAPAR = ptapar;
        MCF_GPIO_PTCPAR = ptcpar;
#if ! DEMO
        MCF_GPIO_PUAPAR = puapar;
#endif
        MCF_GPIO_PUBPAR = pubpar;
        MCF_GPIO_PUCPAR = pucpar;

        // reinitialize the adc
        adc_initialize();
    }
}

// this function initializes the sleep module.
void
sleep_initialize(void)
{
    // NQ is primary (irq1)
    MCF_GPIO_PNQPAR = (MCF_GPIO_PNQPAR &~ (3<<(1*2))) | (1<<(1*2));  // irq1 is primary

    // we're awake; program irq1 for level trigger
    MCF_EPORT_EPPAR = (MCF_EPORT_EPPAR &~ MCF_EPORT_EPPAR_EPPA1_BOTH) | MCF_EPORT_EPPAR_EPPA1_LEVEL;
    MCF_EPORT_EPIER |= MCF_EPORT_EPIER_EPIE1;

    // enable irq1 interrupt
    MCF_INTC0_ICR01 = MCF_INTC_ICR_IL(SPL_IRQ1)|MCF_INTC_ICR_IP(SPL_IRQ1);
    MCF_INTC0_IMRL &= ~MCF_INTC_IMRL_MASKALL;
    MCF_INTC0_IMRL &= ~MCF_INTC_IMRL_INT_MASK1;  // irq1
}
#endif

