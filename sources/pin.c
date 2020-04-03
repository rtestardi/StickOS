// *** pin.c **********************************************************
// this file implements basic I/O pin controls.

#include "main.h"

#define SHRINK  0

#if FLASHER
#define pin_get_digital_debounced(a, b)  0
#endif

int servo_hz = 45;

#define SERVO_MAX (1000000/servo_hz)  // microseconds
#if MC9S08QE128 || MCF51QE128 || MCF51CN128 || MCF51JM128 || PIC32
#if PIC32
#define SERVO_PRESCALE  64
#else
#define SERVO_PRESCALE  16
#endif
#define SERVO_MOD  (bus_frequency/SERVO_PRESCALE/servo_hz)
#elif MC9S12DT256 || MC9S12DP512
#define SERVO_PRESCALE  8
#else
#define SERVO_PRESCALE  16
#define SERVO_MOD  (bus_frequency/SERVO_PRESCALE/servo_hz)
#endif

const char * const pin_assignment_names[] = {
    "heartbeat",
    "safemode*",
    "qspi_cs*",  // clone and zigflea
    "clone_rst*",
    "zigflea_rst*",
    "zigflea_attn*",
    "zigflea_rxtxen",
#if LCD
    "lcd_d4",
    "lcd_d5",
    "lcd_d6",
    "lcd_d7",
    "lcd_en",
    "lcd_rs",
#endif
#if KBD
    "kbd_s0",
    "kbd_s1",
    "kbd_s2",
    "kbd_s3",
    "kbd_r0",
    "kbd_r1",
    "kbd_r2",
    "kbd_r3",
#endif
};

byte pin_assignments[pin_assignment_max] = {
    // set our default pin assignments
#if MC9S08QE128 || MCF51QE128
    PIN_PTC2, PIN_PTA2, PIN_PTB5, PIN_UNASSIGNED, PIN_PTC0, PIN_PTC1, PIN_PTF1,
#elif MCF51CN128
    PIN_PTE3, PIN_PTG6, PIN_PTF0, PIN_UNASSIGNED, PIN_PTF1, PIN_PTF2, PIN_PTF3,
#elif MC9S12DT256 || MC9S12DP512
    PIN_PB7, PIN_PP0, PIN_PM3, PIN_UNASSIGNED, PIN_PT0, PIN_PT1, PIN_PB6,
#elif MCF51JM128
#if FB32 || FB32LITE
    PIN_PTE6, PIN_PTG0, PIN_PTE7, PIN_UNASSIGNED, PIN_UNASSIGNED, PIN_UNASSIGNED, PIN_UNASSIGNED,
#else
    PIN_PTF0, PIN_PTG0, PIN_PTE7, PIN_UNASSIGNED, PIN_PTE2, PIN_PTE3, PIN_PTB5,
#endif
#elif MCF5211 || MCF52221 || MCF52233 || (MCF52259 && DEMO)
#if MCF52259 && FSLBOT
    PIN_UNASSIGNED,
#else
    PIN_DTIN3,
#endif
#if MCF5211
      PIN_IRQ4,
#elif MCF52221 || MCF52233
      PIN_IRQ1,
#elif MCF52259
      PIN_IRQ5,
#endif
        PIN_QSPI_CS0,
          PIN_SCL,
#if MCF5211 || MCF52233
            PIN_GPT0, PIN_GPT1, PIN_AN5,
#else
            PIN_AN2, PIN_AN3, PIN_AN5,
#endif
#elif MCF52259
    PIN_FEC_CRS, PIN_FEC_COL, PIN_QSPI_CS0, PIN_QSPI_CS2, PIN_FEC_RXER, PIN_UNASSIGNED, PIN_FEC_TXCLK,
#elif PIC32
#if DUINOMITE
    PIN_AN15, PIN_RE6, PIN_RE1, PIN_UNASSIGNED, PIN_RE2, PIN_RE3, PIN_RE4,
#elif CHIPKIT
#if defined(__32MX320F128H__)
    PIN_RG6, PIN_AN13, PIN_RG9, PIN_UNASSIGNED, PIN_UNASSIGNED, PIN_UNASSIGNED, PIN_UNASSIGNED,
#else
    PIN_RA3, PIN_AN13, PIN_RG9, PIN_UNASSIGNED, PIN_UNASSIGNED, PIN_UNASSIGNED, PIN_UNASSIGNED,
#endif
#else
    PIN_RE0, PIN_RE6, PIN_RE1, PIN_UNASSIGNED, PIN_RE2, PIN_RE3, PIN_RE4,
#endif
#else
#error
#endif

#if LCD
#if FB32 || FB32LITE
    PIN_PTA2, PIN_PTA3, PIN_PTA4, PIN_PTA5, PIN_PTA1, PIN_PTA0,
#else
    PIN_UNASSIGNED, PIN_UNASSIGNED, PIN_UNASSIGNED, PIN_UNASSIGNED, PIN_UNASSIGNED, PIN_UNASSIGNED,
#endif
#endif

#if KBD
#if FB32 || FB32LITE
    PIN_PTD4, PIN_PTD5, PIN_PTD6, PIN_PTD7, PIN_PTD0, PIN_PTD1, PIN_PTD2, PIN_PTD3
#else
    PIN_UNASSIGNED, PIN_UNASSIGNED, PIN_UNASSIGNED, PIN_UNASSIGNED, PIN_UNASSIGNED, PIN_UNASSIGNED, PIN_UNASSIGNED, PIN_UNASSIGNED
#endif
#endif
};

const char * const pin_type_names[] = {
    "digital input",
    "digital output",
    "analog input",
    "analog output",
    "uart input",
    "uart output",
    "frequency output",
    "servo output"
};

byte const pin_qual_mask[] = {
    1<<pin_qual_inverted | 1<<pin_qual_debounced,  // digital input
    1<<pin_qual_inverted | 1<<pin_qual_open_drain,  // digital output
    1<<pin_qual_inverted | 1<<pin_qual_debounced,  // analog input
    1<<pin_qual_inverted,  // analog output
    0,  // uart input
    0,  // uart output
    0,  // frequency output
    1<<pin_qual_inverted  // servo output
};

// Keep in-sync with pin_qual.  Each element in this array corresponds to a bit in pin_qual.
const char * const pin_qual_names[] = {
    "debounced",
    "inverted",
    "open_drain"
};

int pin_last = PIN_LAST;

#if FB32 || FB32LITE
int32 pin_analog = 5000;
#else
int32 pin_analog = 3300;
#endif

static byte declared[(PIN_UNASSIGNED+7)/8];

#define DIO  (1<<pin_type_digital_output|1<<pin_type_digital_input)

const struct pin pins[] = {
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    "dtin0", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,
    "dtin1", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,
    "dtin2", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,
    "dtin3", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,
    "qspi_dout", DIO,
    "qspi_din", DIO,
    "qspi_clk", DIO,
    "qspi_cs0", DIO,
#if MCF52259
    "qspi_cs1", 0,
    "qspi_cs2", DIO,
    "qspi_cs3", DIO,
    "fec_col", DIO,
    "fec_crs", DIO,
    "fec_rxclk", DIO,
    "fec_rxd0", DIO,
    "fec_rxd1", DIO,
    "fec_rxd2", DIO,
    "fec_rxd3", DIO,
    "fec_rxdv", DIO,
    "fec_rxer", DIO,
    "fec_txclk", DIO,
    "fec_txd0", DIO,
    "fec_txd1", DIO,
    "fec_txd2", DIO,
    "fec_txd3", DIO,
    "fec_txen", DIO,
    "fec_txer", DIO,
    "utxd2", DIO|1<<pin_type_uart_output,
    "urxd2", DIO|1<<pin_type_uart_input,
    "urts2*", DIO,
    "ucts2*", DIO,
#endif
    "utxd1", DIO|1<<pin_type_uart_output,
    "urxd1", DIO|1<<pin_type_uart_input,
    "urts1*", DIO,
    "ucts1*", DIO,
    "utxd0", DIO|1<<pin_type_uart_output,
    "urxd0", DIO|1<<pin_type_uart_input,
    "urts0*", DIO,
    "ucts0*", DIO,
    "an0", DIO|1<<pin_type_analog_input,
    "an1", DIO|1<<pin_type_analog_input,
    "an2", DIO|1<<pin_type_analog_input,
    "an3", DIO|1<<pin_type_analog_input,
    "an4", DIO|1<<pin_type_analog_input,
    "an5", DIO|1<<pin_type_analog_input,
    "an6", DIO|1<<pin_type_analog_input,
    "an7", DIO|1<<pin_type_analog_input,
#if MCF52259
    "irq0*", 0,
    "irq1*", DIO,
    "irq2*", 0,
    "irq3*", DIO,
    "irq4*", 0,
    "irq5*", DIO,
    "irq6*", 0,
    "irq7*", DIO,
#else
    "irq0*", 0,
    "irq1*", DIO,
    "irq2*", 0,
    "irq3*", 0,
    "irq4*", DIO,
    "irq5*", 0,
    "irq6*", 0,
    "irq7*", DIO,
#endif
#if MCF52233
    "irq8*", 0,
    "irq9*", 0,
    "irq10*", 0,
    "irq11*", DIO,
#endif
#if MCF52233 || MCF52259 || MCF5211
    "gpt0", DIO,
    "gpt1", DIO,
    "gpt2", DIO,
    "gpt3", DIO,
#endif
    "scl", DIO,
    "sda", DIO,
#elif MCF51JM128
#if FB32
// FB32
    "pta0", DIO,
    "pta1", DIO,
    "pta2", DIO,
    "pta3", DIO,
    "pta4", DIO,
    "pta5", DIO,
    "a0", DIO|1<<pin_type_analog_input,
    "a1", DIO|1<<pin_type_analog_input,
    "a2", DIO|1<<pin_type_analog_input,
    "a3", DIO|1<<pin_type_analog_input,
    "a4", DIO|1<<pin_type_analog_input,
    "a5", DIO|1<<pin_type_analog_input,
    "a6", DIO|1<<pin_type_analog_input,
    "a7", DIO|1<<pin_type_analog_input,
    "ptc0", DIO,
    "ptc1", DIO,
    "ptc2", DIO,
    "ptc3", DIO|1<<pin_type_uart_output,
    "ptc4", DIO,
    "ptc5", DIO|1<<pin_type_uart_input,
    "ptc6", DIO,
    "ptd0", DIO|1<<pin_type_analog_input,
    "7", DIO|1<<pin_type_analog_input,
    "4", DIO,
    "ptd3", DIO|1<<pin_type_analog_input,
    "ptd4", DIO|1<<pin_type_analog_input,
    "ptd5", DIO,
    "ptd6", DIO,
    "ptd7", DIO,
    "1", DIO|1<<pin_type_uart_output,
    "0", DIO|1<<pin_type_uart_input,
    "3", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,
    "5", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,
    "12", DIO,
    "pte5", DIO,
    "13", DIO,
    "pte7", DIO,
    "6", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,
    "9", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,
    "10", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,
    "11", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,
    "8", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,
    "ptf5", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,
    "ptf6", DIO,
    "ptf7", DIO,
    "ptg0", DIO,
    "ptg1", DIO,
    "ptg2", DIO,
    "2", DIO,
#else
    "pta0", DIO,
    "pta1", DIO,
    "pta2", DIO,
    "pta3", DIO,
    "pta4", DIO,
    "pta5", DIO,
    "ptb0", DIO|1<<pin_type_analog_input,
    "ptb1", DIO|1<<pin_type_analog_input,
    "ptb2", DIO|1<<pin_type_analog_input,
    "ptb3", DIO|1<<pin_type_analog_input,
    "ptb4", DIO|1<<pin_type_analog_input,
    "ptb5", DIO|1<<pin_type_analog_input,
    "ptb6", DIO|1<<pin_type_analog_input,
    "ptb7", DIO|1<<pin_type_analog_input,
    "ptc0", DIO,
    "ptc1", DIO,
    "ptc2", DIO,
    "ptc3", DIO|1<<pin_type_uart_output,
    "ptc4", DIO,
    "ptc5", DIO|1<<pin_type_uart_input,
    "ptc6", DIO,
    "ptd0", DIO|1<<pin_type_analog_input,
    "ptd1", DIO|1<<pin_type_analog_input,
    "ptd2", DIO,
    "ptd3", DIO|1<<pin_type_analog_input,
    "ptd4", DIO|1<<pin_type_analog_input,
    "ptd5", DIO,
    "ptd6", DIO,
    "ptd7", DIO,
    "pte0", DIO|1<<pin_type_uart_output,
    "pte1", DIO|1<<pin_type_uart_input,
    "pte2", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,
    "pte3", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,
    "pte4", DIO,
    "pte5", DIO,
    "pte6", DIO,
    "pte7", DIO,
    "ptf0", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,
    "ptf1", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,
    "ptf2", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,
    "ptf3", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,
    "ptf4", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,
    "ptf5", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,
    "ptf6", DIO,
    "ptf7", DIO,
    "ptg0", DIO,
    "ptg1", DIO,
    "ptg2", DIO,
    "ptg3", DIO,
#endif
#elif MCF51CN128
    "pta0", DIO,
    "pta1", DIO,
    "pta2", DIO,
    "pta3", DIO|1<<pin_type_uart_output,
    "pta4", DIO|1<<pin_type_uart_input,
    "pta5", DIO,
    "pta6", DIO,
    "pta7", DIO,
    "ptb0", DIO,
    "ptb1", DIO,
    "ptb2", DIO,
    "ptb3", DIO,
    "ptb4", DIO,
    "ptb5", DIO,
    "ptb6", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,
    "ptb7", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,
    "ptc0", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,
    "ptc1", DIO,
    "ptc2", DIO,
    "ptc3", 0,
    "ptc4", DIO|1<<pin_type_analog_input,
    "ptc5", DIO|1<<pin_type_analog_input,
    "ptc6", DIO|1<<pin_type_analog_input,
    "ptc7", DIO|1<<pin_type_analog_input,
    "ptd0", DIO|1<<pin_type_analog_input|1<<pin_type_uart_output,
    "ptd1", DIO|1<<pin_type_analog_input|1<<pin_type_uart_input,
    "ptd2", DIO|1<<pin_type_analog_input|1<<pin_type_uart_output,
    "ptd3", DIO|1<<pin_type_analog_input|1<<pin_type_uart_input,
    "ptd4", 0,
    "ptd5", 0,
    "ptd6", DIO,
    "ptd7", DIO|1<<pin_type_analog_input,
    "pte0", DIO|1<<pin_type_analog_input,
    "pte1", DIO|1<<pin_type_analog_input,
    "pte2", DIO|1<<pin_type_analog_input,
    "pte3", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,
    "pte4", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,
    "pte5", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,
    "pte6", DIO,
    "pte7", DIO,
    "ptf0", DIO,
    "ptf1", DIO,
    "ptf2", DIO,
    "ptf3", DIO,
    "ptf4", DIO,
    "ptf5", DIO,
    "ptf6", DIO,
    "ptf7", DIO,
    "ptg0", DIO,
    "ptg1", DIO,
    "ptg2", DIO,
    "ptg3", DIO,
    "ptg4", DIO,
    "ptg5", DIO,
    "ptg6", DIO,
    "ptg7", DIO,
    "pth0", DIO,
    "pth1", DIO,
    "pth2", DIO,
    "pth3", DIO,
    "pth4", DIO,
    "pth5", DIO,
    "pth6", DIO,
    "pth7", DIO,
    "ptj0", DIO,
    "ptj1", DIO,
    "ptj2", DIO,
    "ptj3", DIO,
    "ptj4", DIO,
    "ptj5", DIO,
#elif MCF51QE128 || MC9S08QE128
    "pta0", DIO|1<<pin_type_analog_input|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,
    "pta1", DIO|1<<pin_type_analog_input|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,
    "pta2", DIO|1<<pin_type_analog_input,
    "pta3", DIO|1<<pin_type_analog_input,
    "pta4", DIO,
    "pta5", DIO,
    "pta6", DIO|1<<pin_type_analog_input|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,
    "pta7", DIO|1<<pin_type_analog_input|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,
    "ptb0", DIO|1<<pin_type_analog_input|1<<pin_type_uart_input,
    "ptb1", DIO|1<<pin_type_analog_input|1<<pin_type_uart_output,
    "ptb2", DIO|1<<pin_type_analog_input,
    "ptb3", DIO|1<<pin_type_analog_input,
    "ptb4", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,
    "ptb5", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,
    "ptb6", DIO,
    "ptb7", DIO,
    "ptc0", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,
    "ptc1", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,
    "ptc2", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,
    "ptc3", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,
    "ptc4", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,
    "ptc5", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,
    "ptc6", DIO|1<<pin_type_uart_input,
    "ptc7", DIO|1<<pin_type_uart_output,
    "ptd0", DIO,
    "ptd1", DIO,
    "ptd2", DIO,
    "ptd3", DIO,
    "ptd4", DIO,
    "ptd5", DIO,
    "ptd6", DIO,
    "ptd7", DIO,
    "pte0", DIO,
    "pte1", DIO,
    "pte2", DIO,
    "pte3", DIO,
    "pte4", DIO,
    "pte5", DIO,
    "pte6", DIO,
    "pte7", DIO,
    "ptf0", DIO|1<<pin_type_analog_input,
    "ptf1", DIO|1<<pin_type_analog_input,
    "ptf2", DIO|1<<pin_type_analog_input,
    "ptf3", DIO|1<<pin_type_analog_input,
    "ptf4", DIO|1<<pin_type_analog_input,
    "ptf5", DIO|1<<pin_type_analog_input,
    "ptf6", DIO|1<<pin_type_analog_input,
    "ptf7", DIO|1<<pin_type_analog_input,
    "ptg0", DIO,
    "ptg1", DIO,
    "ptg2", DIO|1<<pin_type_analog_input,
    "ptg3", DIO|1<<pin_type_analog_input,
#elif MC9S12DT256 || MC9S12DP512
    "pad00", 1<<pin_type_digital_input|1<<pin_type_analog_input,
    "pad01", 1<<pin_type_digital_input|1<<pin_type_analog_input,
    "pad02", 1<<pin_type_digital_input|1<<pin_type_analog_input,
    "pad03", 1<<pin_type_digital_input|1<<pin_type_analog_input,
    "pad04", 1<<pin_type_digital_input|1<<pin_type_analog_input,
    "pad05", 1<<pin_type_digital_input|1<<pin_type_analog_input,
    "pad06", 1<<pin_type_digital_input|1<<pin_type_analog_input,
    "pad07", 1<<pin_type_digital_input|1<<pin_type_analog_input,
    "pad08", 1<<pin_type_digital_input|1<<pin_type_analog_input,
    "pad09", 1<<pin_type_digital_input|1<<pin_type_analog_input,
    "pad10", 1<<pin_type_digital_input|1<<pin_type_analog_input,
    "pad11", 1<<pin_type_digital_input|1<<pin_type_analog_input,
    "pad12", 1<<pin_type_digital_input|1<<pin_type_analog_input,
    "pad13", 1<<pin_type_digital_input|1<<pin_type_analog_input,
    "pad14", 1<<pin_type_digital_input|1<<pin_type_analog_input,
    "pad15", 1<<pin_type_digital_input|1<<pin_type_analog_input,
    "pa0", DIO,
    "pa1", DIO,
    "pa2", DIO,
    "pa3", DIO,
    "pa4", DIO,
    "pa5", DIO,
    "pa6", DIO,
    "pa7", DIO,
    "pb0", DIO,
    "pb1", DIO,
    "pb2", DIO,
    "pb3", DIO,
    "pb4", DIO,
    "pb5", DIO,
    "pb6", DIO,
    "pb7", DIO,
    "pe0", 1<<pin_type_digital_input,
    "pe1", 1<<pin_type_digital_input,
    "pe2", DIO,
    "pe3", DIO,
    "pe4", DIO,
    "pe5", DIO,
    "pe6", DIO,
    "pe7", DIO,
    "ph0", DIO,
    "ph1", DIO,
    "ph2", DIO,
    "ph3", DIO,
    "ph4", DIO,
    "ph5", DIO,
    "ph6", DIO,
    "ph7", DIO,
    "pj0", DIO,
    "pj1", DIO,
    "pj2", 0,
    "pj3", 0,
    "pj4", 0,
    "pj5", 0,
    "pj6", DIO,
    "pj7", DIO,
    "pk0", DIO,
    "pk1", DIO,
    "pk2", DIO,
    "pk3", DIO,
    "pk4", DIO,
    "pk5", DIO,
    "pk6", DIO,
    "pk7", DIO,
    "pm0", DIO,
    "pm1", DIO,
    "pm2", DIO,
    "pm3", DIO,
    "pm4", DIO,
    "pm5", DIO,
    "pm6", DIO,
    "pm7", DIO,
    "pp0", DIO|1<<pin_type_analog_output|1<<pin_type_servo_output,
    "pp1", DIO|1<<pin_type_analog_output|1<<pin_type_servo_output,
    "pp2", DIO|1<<pin_type_analog_output|1<<pin_type_servo_output,
    "pp3", DIO|1<<pin_type_analog_output|1<<pin_type_servo_output,
    "pp4", DIO|1<<pin_type_analog_output|1<<pin_type_servo_output,
    "pp5", DIO|1<<pin_type_analog_output|1<<pin_type_servo_output,
    "pp6", DIO|1<<pin_type_analog_output|1<<pin_type_servo_output,
    "pp7", DIO|1<<pin_type_analog_output|1<<pin_type_servo_output,
    "ps0", DIO|1<<pin_type_uart_input,
    "ps1", DIO|1<<pin_type_uart_output,
    "ps2", DIO|1<<pin_type_uart_input,
    "ps3", DIO|1<<pin_type_uart_output,
    "ps4", DIO,
    "ps5", DIO,
    "ps6", DIO,
    "ps7", DIO,
    "pt0", DIO|1<<pin_type_frequency_output,
    "pt1", DIO|1<<pin_type_frequency_output,
    "pt2", DIO|1<<pin_type_frequency_output,
    "pt3", DIO|1<<pin_type_frequency_output,
    "pt4", DIO|1<<pin_type_frequency_output,
    "pt5", DIO|1<<pin_type_frequency_output,
    "pt6", DIO|1<<pin_type_frequency_output,
    "pt7", DIO|1<<pin_type_frequency_output,
#elif PIC32
#if defined(__32MX320F128H__) && defined CHIPKIT
// UNO32
#ifdef PIC32PORTA
    "ra0", DIO,
    "ra1", DIO,
    "ra2", DIO,
    "ra3", DIO,
    "ra4", DIO,
    "ra5", DIO,
    "ra6", DIO,
    "ra7", DIO,
    "ra8", 0,
    "ra9", DIO,
    "ra10", DIO,
    "ra11", 0,
    "ra12", 0,
    "ra13", 0,
    "ra14", DIO,
    "ra15", DIO,
#endif
    "an0", DIO|1<<pin_type_analog_input,  // rb0...
    "41", DIO|1<<pin_type_analog_input,
    "a0", DIO|1<<pin_type_analog_input,
    "a6", DIO|1<<pin_type_analog_input,
    "a1", DIO|1<<pin_type_analog_input,
    "a7", DIO|1<<pin_type_analog_input,
    "rb6", DIO|1<<pin_type_analog_input,
    "rb7", DIO|1<<pin_type_analog_input,
    "a2", DIO|1<<pin_type_analog_input,  // U2CTS
    "a8", DIO|1<<pin_type_analog_input,
    "a3", DIO|1<<pin_type_analog_input,
    "a9", DIO|1<<pin_type_analog_input,
    "a4", DIO|1<<pin_type_analog_input,
    "a10", DIO|1<<pin_type_analog_input,
    "a5", DIO|1<<pin_type_analog_input,  // U2RTS
    "a11", DIO|1<<pin_type_analog_input,
    "rc0", 0,
    "rc1", DIO,  // rc1...
    "rc2", DIO,
    "rc3", DIO,
    "rc4", DIO,
    "rc5", 0,
    "rc6", 0,
    "rc7", 0,
    "rc8", 0,
    "rc9", 0,
    "rc10", 0,
    "rc11", 0,
    "rc12", DIO,
    "rc13", DIO,  // rc13...
    "rc14", DIO,
    "rc15", DIO,
    "3", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,  // oc1
    "5", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,  // oc2
    "6", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,  // oc3
    "9", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,  // oc4
    "10", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,  // oc5
    "34", DIO,
    "36", DIO,
    "37", DIO,
    "2", DIO,
    "7", DIO,
    "8", DIO,
    "35", DIO,
    "rd12", DIO,
    "rd13", DIO,
    "rd14", DIO,
    "rd15", DIO,
    "26", DIO,
    "27", DIO,
    "28", DIO,
    "29", DIO,
    "30", DIO,
    "31", DIO,
    "32", DIO,
    "33", DIO,
    "re8", DIO,
    "re9", DIO,
    "43", DIO,
    "4", DIO,
    "0", DIO,
    "1", DIO,
    "39", DIO|1<<pin_type_uart_input,  // u2rx
    "40", DIO|1<<pin_type_uart_output,  // u2tx
    "38", DIO,
    "rf7", 0,
    "rf8", DIO,
    "rf9", 0,
    "rf10", 0,
    "rf11", 0,
    "rf12", DIO,
    "rf13", DIO,
    "rg0", DIO,
    "rg1", DIO,
    "rg2", DIO,
    "rg3", DIO,
    "rg4", 0,
    "rg5", 0,
    "13", DIO,
    "12", DIO,
    "11", DIO,
    "rg9", DIO,
    "rg10", 0,
    "rg11", 0,
    "rg12", DIO,
    "rg13", DIO,
    "rg14", DIO,
    "rg15", DIO,
#elif defined(__32MX795F512L__) && defined CHIPKIT
// MAX32
#ifdef PIC32PORTA
    "70", DIO,
    "71", DIO,
    "12", DIO,
    "13", DIO,
    "72", DIO,
    "73", DIO,
    "80", DIO,
    "81", DIO,
    "ra8", 0,
    "85", DIO,
    "44", DIO,
    "ra11", 0,
    "ra12", 0,
    "ra13", 0,
    "21", DIO,
    "20", DIO,
#endif
    "a0", DIO|1<<pin_type_analog_input,  // rb0...
    "a1", DIO|1<<pin_type_analog_input,
    "a2", DIO|1<<pin_type_analog_input,
    "a3", DIO|1<<pin_type_analog_input,
    "a4", DIO|1<<pin_type_analog_input,
    "a5", DIO|1<<pin_type_analog_input,
    "a6", DIO|1<<pin_type_analog_input,
    "a7", DIO|1<<pin_type_analog_input,
    "a8", DIO|1<<pin_type_analog_input,  // U2CTS
    "a9", DIO|1<<pin_type_analog_input,
    "a10", DIO|1<<pin_type_analog_input,
    "a11", DIO|1<<pin_type_analog_input,
    "a12", DIO|1<<pin_type_analog_input,
    "a13", DIO|1<<pin_type_analog_input,
    "a14", DIO|1<<pin_type_analog_input,  // U2RTS
    "a15", DIO|1<<pin_type_analog_input,
    "rc0", 0,
    "rc1", DIO,  // rc1...
    "22", DIO,
    "23", DIO,
    "11", DIO,
    "rc5", 0,
    "rc6", 0,
    "rc7", 0,
    "rc8", 0,
    "rc9", 0,
    "rc10", 0,
    "rc11", 0,
    "rc12", DIO,
    "75", DIO,  // rc13...
    "4", DIO,
    "rc15", DIO,
    "3", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,  // oc1
    "5", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,  // oc2
    "6", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,  // oc3
    "9", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,  // oc4
    "10", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,  // oc5
    "39", DIO,
    "47", DIO,
    "77", DIO,
    "48", DIO,
    "74", DIO,
    "38", DIO,
    "49", DIO,
    "8", DIO,
    "76", DIO,
    "19", DIO,
    "18", DIO,
    "37", DIO,
    "36", DIO,
    "35", DIO,
    "34", DIO,
    "33", DIO,
    "32", DIO,
    "31", DIO,
    "30", DIO,
    "2", DIO,
    "7", DIO,
    "45", DIO,
    "46", DIO,
    "0", DIO,
    "rf3", DIO,
    "17", DIO|1<<pin_type_uart_input,  // u2rx
    "16", DIO|1<<pin_type_uart_output,  // u2tx
    "rf6", DIO,
    "rf7", 0,
    "1", DIO,
    "rf9", 0,
    "rf10", 0,
    "rf11", 0,
    "15", DIO,
    "14", DIO,
    "79", DIO,
    "78", DIO,
    "rg2", DIO,
    "rg3", DIO,
    "rg4", 0,
    "rg5", 0,
    "52", DIO,
    "29", DIO,
    "43", DIO,
    "53", DIO,
    "rg10", 0,
    "rg11", 0,
    "83", DIO,
    "84", DIO,
    "82", DIO,
    "28", DIO,
#else
#ifdef PIC32PORTA
    "ra0", DIO,
    "ra1", DIO,
    "ra2", DIO,
    "ra3", DIO,
    "ra4", DIO,
    "ra5", DIO,
    "ra6", DIO,
    "ra7", DIO,
    "ra8", 0,
    "ra9", DIO,
    "ra10", DIO,
    "ra11", 0,
    "ra12", 0,
    "ra13", 0,
    "ra14", DIO,
    "ra15", DIO,
#endif
    "an0", DIO|1<<pin_type_analog_input,  // rb0...
    "an1", DIO|1<<pin_type_analog_input,
    "an2", DIO|1<<pin_type_analog_input,
    "an3", DIO|1<<pin_type_analog_input,
    "an4", DIO|1<<pin_type_analog_input,
    "an5", DIO|1<<pin_type_analog_input,
    "an6", DIO|1<<pin_type_analog_input,
    "an7", DIO|1<<pin_type_analog_input,
    "an8", DIO|1<<pin_type_analog_input,  // U2CTS
    "an9", DIO|1<<pin_type_analog_input,
    "an10", DIO|1<<pin_type_analog_input,
    "an11", DIO|1<<pin_type_analog_input,
    "an12", DIO|1<<pin_type_analog_input,
    "an13", DIO|1<<pin_type_analog_input,
    "an14", DIO|1<<pin_type_analog_input,  // U2RTS
    "an15", DIO|1<<pin_type_analog_input,
    "rc0", 0,
    "rc1", DIO,  // rc1...
    "rc2", DIO,
    "rc3", DIO,
    "rc4", DIO,
    "rc5", 0,
    "rc6", 0,
    "rc7", 0,
    "rc8", 0,
    "rc9", 0,
    "rc10", 0,
    "rc11", 0,
    "rc12", DIO,
    "rc13", DIO,  // rc13...
    "rc14", DIO,
    "rc15", DIO,
    "rd0", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,  // oc1
    "rd1", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,  // oc2
    "rd2", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,  // oc3
    "rd3", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,  // oc4
    "rd4", DIO|1<<pin_type_analog_output|1<<pin_type_frequency_output|1<<pin_type_servo_output,  // oc5
    "rd5", DIO,
    "rd6", DIO,
    "rd7", DIO,
    "rd8", DIO,
    "rd9", DIO,
    "rd10", DIO,
    "rd11", DIO,
    "rd12", DIO,
    "rd13", DIO,
    "rd14", DIO,
    "rd15", DIO,
    "re0", DIO,
    "re1", DIO,
    "re2", DIO,
    "re3", DIO,
    "re4", DIO,
    "re5", DIO,
    "re6", DIO,
    "re7", DIO,
    "re8", DIO,
    "re9", DIO,
    "rf0", DIO,
    "rf1", DIO,
    "rf2", DIO,
    "rf3", DIO,
    "rf4", DIO|1<<pin_type_uart_input,  // u2rx
    "rf5", DIO|1<<pin_type_uart_output,  // u2tx
    "rf6", DIO,
    "rf7", 0,
    "rf8", DIO,
    "rf9", 0,
    "rf10", 0,
    "rf11", 0,
    "rf12", DIO,
    "rf13", DIO,
    "rg0", DIO,
    "rg1", DIO,
    "rg2", DIO,
    "rg3", DIO,
    "rg4", 0,
    "rg5", 0,
    "rg6", DIO,
    "rg7", DIO,
    "rg8", DIO,
    "rg9", DIO,
    "rg10", 0,
    "rg11", 0,
    "rg12", DIO,
    "rg13", DIO,
    "rg14", DIO,
    "rg15", DIO,
#endif
#else
#error
#endif
    "none", 0,
};

bool uart_armed[UART_INTS];

const char * const uart_names[MAX_UARTS] = {
#if MCF52221 || MCF52233 || MCF52259 || MCF5211 || MC9S12DT256 || MC9S12DP512
    "0",
    "1",
#if MCF52259
    "2",
#endif
#else
    "1",
    "2",
#if MCF51CN128
    "3",
#endif
#endif
};

#if MCF51JM128 || MCF51CN128 || PIC32
static byte freq[2];  // 0, pin_type_analog_output, pin_type_servo_output, pin_type_frequency_output
#elif MCF51QE128 || MC9S08QE128
static byte freq[3];  // 0, pin_type_analog_output, pin_type_servo_output, pin_type_frequency_output
#elif MC9S12DT256 || MC9S12DP512
static byte freq[1];  // 0 or pin_type_frequency_output
#endif

#if MCF51JM128 || MCF51CN128 || MCF51QE128 || MC9S08QE128 || MC9S12DT256 || MC9S12DP512
#define FREQ_PRESCALE  16
#elif PIC32
#define FREQ_PRESCALE  64
#endif

#if ! STICK_GUEST && ! FLASHER

// Debounce history for digital inputs.

enum {
    // Tunable depth of digital pin history saved for debouncing.  Currently, the history is used to elect a majority.
    pin_digital_debounce_history_depth = 3
};

enum debounce_ports {
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    port_tc,
    port_qs,
#if MCF52259
    port_ti,
    port_tj,
    port_uc,
#endif
    port_ub,
    port_ua,
    port_an,
    port_nq,
    port_as,
#if MCF52233
    port_gp,
#endif
#if MCF52233 || MCF52259 || MCF5211
    port_ta,
#endif
#elif MCF51JM128 || MCF51CN128 || MCF51QE128 || MC9S08QE128
    port_a,
    port_b,
    port_c,
    port_d,
    port_e,
    port_f,
    port_g,
#if MCF51CN128
    port_h,
    port_j,
#endif
#elif MC9S12DT256 || MC9S12DP512
    port_ad0,
    port_ad1,
    port_a,
    port_b,
    port_e,
    port_j,
    port_m,
    port_p,
    port_s,
    port_t,
#elif PIC32
#ifdef PIC32PORTA
    port_a,
#endif
    port_b,
#if PIC32PPS
#else
    port_c,
    port_d,
    port_e,
    port_f,
    port_g,
#endif
#else
#error
#endif
    port_max
};

// This structure records recent samples from digital pins.
#if MCF52221 || MCF52233 || MCF52259 || MCF5211 || MCF51JM128 || MCF51CN128 || MCF51QE128 || MC9S08QE128 || MC9S12DT256 || MC9S12DP512
typedef uint8 pin_port_sample_t;
#elif PIC32
typedef uint16 pin_port_sample_t;
#else
#error
#endif

static pin_port_sample_t pin_digital_debounce[pin_digital_debounce_history_depth][port_max];

static int pin_digital_debounce_cycle; // indexes into pin_digital_debounce_data.

// compute majority value of the pin's recently polled values.
//
// revisit -- find a way to rework and rename this to
// pin_get_digital() and have it returned debounced or non-debounced
// data based on pin_qual.  Also introduce a pin_set_digital() to
// consolidate setting of pins.
static int
pin_get_digital_debounced(int port_offset, int pin_offset)
{
    int i;
    int value;
    
    assert(pin_offset < sizeof(pin_digital_debounce[0][0]) * 8);

    value = 0;
    for (i = 0; i < pin_digital_debounce_history_depth; i++) {
        value += !!(pin_digital_debounce[i][port_offset] & (1 << pin_offset));
    }
    return value > pin_digital_debounce_history_depth/2;
}

#endif // ! STICK_GUEST && ! FLASHER


static
void
pin_declare_internal(IN int pin_number, IN int pin_type, IN int pin_qual, IN bool set, IN bool user)
{
#if ! STICK_GUEST && ! SHRINK
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    int assign;
#endif
#if MCF51JM128 || MCF51CN128 || MCF51QE128 || MC9S08QE128 || MC9S12DT256 || MC9S12DP512
#if ! MC9S12DT256 && ! MC9S12DP512
    int n;
    int mod;
    int mask;
#endif
#if ! MCF51CN128
    uint32 adc;
#endif
#endif
    uint32 offset;
    
    assert(pin_number != PIN_UNASSIGNED);
    if (user) {
        declared[pin_number/8] |= 1 << (pin_number%8);
    }

    if (! set && (pin_type == pin_type_digital_output) && (pin_qual & 1<<pin_qual_open_drain)) {
        // on initial declaration, configure open_drain outputs as inputs
        // N.B. this will be reconfigured as an output on pin_set to 0
        pin_type = pin_type_digital_input;
    }

#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    // configure the MCF52221/MCF52233/MCF52259/MCF5211 pin for the requested function
    switch (pin_number) {
        case PIN_DTIN0:
        case PIN_DTIN1:
        case PIN_DTIN2:
        case PIN_DTIN3:
            offset = pin_number - PIN_DTIN0;
            assert(offset < 4);
            if (pin_type == pin_type_digital_output || pin_type == pin_type_digital_input) {
                assign = 0;
            } else if (pin_type == pin_type_analog_output) {
                assign = 3;
                MCF_PWM_PWMCLK &= ~(1 << (offset*2+1));  // prescaled clock

                // set periods to pin_analog for analog output
                assert(pin_analog < 0x10000);
                MCF_PWM_PWMPER(offset*2) = pin_analog/256;
                MCF_PWM_PWMPER(offset*2+1) = pin_analog%256;
            } else if (pin_type == pin_type_servo_output) {
                assign = 3;
                MCF_PWM_PWMCLK |= 1 << (offset*2+1);  // prescaled and scaled clock

                // set periods to SERVO_MOD for 45Hz for servo output
                assert(SERVO_MOD < 0x10000);
                MCF_PWM_PWMPER(offset*2) = SERVO_MOD/256;
                MCF_PWM_PWMPER(offset*2+1) = SERVO_MOD%256;
            } else {
                assert(pin_type == pin_type_frequency_output);
                assign = 2;
            }
            MCF_GPIO_PTCPAR = (MCF_GPIO_PTCPAR &~ (3<<(offset*2))) | (assign<<(offset*2));
            if (pin_type == pin_type_digital_output) {
                MCF_GPIO_DDRTC |= 1 << offset;
            } else if (pin_type == pin_type_digital_input) {
                MCF_GPIO_DDRTC &= ~(1 << offset);
            }
            break;
        case PIN_QSPI_DOUT:
        case PIN_QSPI_DIN:
        case PIN_QSPI_CLK:
        case PIN_QSPI_CS0:
#if MCF52259
        case PIN_QSPI_CS2:
        case PIN_QSPI_CS3:
#endif
            offset = pin_number - PIN_QSPI_DOUT;
            assert(offset < 8);
            MCF_GPIO_PQSPAR &= ~(3<<2*offset);
            if (pin_type == pin_type_digital_output) {
                MCF_GPIO_DDRQS |= 1 << offset;
            } else {
                assert(pin_type == pin_type_digital_input);
                MCF_GPIO_DDRQS &= ~(1 << offset);
            }
            break;
#if MCF52259
        case PIN_FEC_COL:
        case PIN_FEC_CRS:
        case PIN_FEC_RXCLK:
        case PIN_FEC_RXD0:
        case PIN_FEC_RXD1:
        case PIN_FEC_RXD2:
        case PIN_FEC_RXD3:
        case PIN_FEC_RXDV:
            offset = pin_number - PIN_FEC_COL;
            assert(offset < 8);
            if (pin_type == pin_type_digital_output) {
                MCF_GPIO_DDRTI |= 1 << offset;
            } else {
                assert(pin_type == pin_type_digital_input);
                MCF_GPIO_DDRTI &= ~(1 << offset);
            }
            break;
        case PIN_FEC_RXER:
        case PIN_FEC_TXCLK:
        case PIN_FEC_TXD0:
        case PIN_FEC_TXD1:
        case PIN_FEC_TXD2:
        case PIN_FEC_TXD3:
        case PIN_FEC_TXEN:
        case PIN_FEC_TXER:
            offset = pin_number - PIN_FEC_RXER;
            assert(offset < 8);
            if (pin_type == pin_type_digital_output) {
                MCF_GPIO_DDRTJ |= 1 << offset;
            } else {
                assert(pin_type == pin_type_digital_input);
                MCF_GPIO_DDRTJ &= ~(1 << offset);
            }
            break;
        case PIN_UTXD2:
        case PIN_URXD2:
        case PIN_RTS2:
        case PIN_CTS2:
            offset = pin_number - PIN_UTXD2;
            assert(offset < 4);
            if (pin_type == pin_type_uart_input || pin_type == pin_type_uart_output) {
                assert(pin_number == PIN_URXD2 || pin_number == PIN_UTXD2);
                MCF_GPIO_PUCPAR = (MCF_GPIO_PUCPAR &~ (3 << (offset*2))) | (1 << (offset*2));
            } else {
                MCF_GPIO_PUCPAR &= ~(3 << (offset*2));
                if (pin_type == pin_type_digital_output) {
                    MCF_GPIO_DDRUC |= 1 << offset;
                } else {
                    assert(pin_type == pin_type_digital_input);
                    MCF_GPIO_DDRUC &= ~(1 << offset);
                }
            }
            break;
#endif
        case PIN_UTXD1:
        case PIN_URXD1:
        case PIN_RTS1:
        case PIN_CTS1:
            offset = pin_number - PIN_UTXD1;
            assert(offset < 4);
            if (pin_type == pin_type_uart_input || pin_type == pin_type_uart_output) {
                assert(pin_number == PIN_URXD1 || pin_number == PIN_UTXD1);
                MCF_GPIO_PUBPAR = (MCF_GPIO_PUBPAR &~ (3 << (offset*2))) | (1 << (offset*2));
            } else {
                MCF_GPIO_PUBPAR &= ~(3 << (offset*2));
                if (pin_type == pin_type_digital_output) {
                    MCF_GPIO_DDRUB |= 1 << offset;
                } else {
                    assert(pin_type == pin_type_digital_input);
                    MCF_GPIO_DDRUB &= ~(1 << offset);
                }
            }
            break;
        case PIN_UTXD0:
        case PIN_URXD0:
        case PIN_RTS0:
        case PIN_CTS0:
            offset = pin_number - PIN_UTXD0;
            assert(offset < 4);
            if (pin_type == pin_type_uart_input || pin_type == pin_type_uart_output) {
                assert(pin_number == PIN_URXD0 || pin_number == PIN_UTXD0);
                MCF_GPIO_PUAPAR = (MCF_GPIO_PUAPAR &~ (3 << (offset*2))) | (1 << (offset*2));
            } else {
                MCF_GPIO_PUAPAR &= ~(3 << (offset*2));
                if (pin_type == pin_type_digital_output) {
                    MCF_GPIO_DDRUA |= 1 << offset;
                } else {
                    MCF_GPIO_DDRUA &= ~(1 << offset);
                    assert(pin_type == pin_type_digital_input);
                }
            }
            break;
        case PIN_AN0:
        case PIN_AN1:
        case PIN_AN2:
        case PIN_AN3:
        case PIN_AN4:
        case PIN_AN5:
        case PIN_AN6:
        case PIN_AN7:
            offset = pin_number - PIN_AN0;
            assert(offset < 8);
            if (pin_type == pin_type_analog_input) {
                MCF_GPIO_PANPAR |= 1 << offset;
            } else {
                MCF_GPIO_PANPAR &= ~(1 << offset);
                if (pin_type == pin_type_digital_output) {
                    MCF_GPIO_DDRAN |= 1 << offset;
                } else {
                    MCF_GPIO_DDRAN &= ~(1 << offset);
                    assert(pin_type == pin_type_digital_input);
                }
            }
            break;
        case PIN_IRQ1:
        case PIN_IRQ2:
        case PIN_IRQ3:
        case PIN_IRQ4:
        case PIN_IRQ5:
        case PIN_IRQ6:
        case PIN_IRQ7:
            offset = pin_number - PIN_IRQ0;
            assert(offset < 8);
#if MCF5211
            if (offset == 1) {
                MCF_GPIO_PNQPAR &= ~3;
            } else {
                MCF_GPIO_PNQPAR &= ~(1<<offset);
            }
#else
            MCF_GPIO_PNQPAR &= ~(3<<(offset*2));
#endif
            if (pin_type == pin_type_digital_output) {
                MCF_GPIO_DDRNQ |= 1 << offset;
            } else {
                assert(pin_type == pin_type_digital_input);
                MCF_GPIO_DDRNQ &= ~(1 << offset);
            }
            break;
#if MCF52233
        case PIN_IRQ11:
            offset = pin_number - PIN_IRQ8;
            assert(offset < 8);
            MCF_GPIO_PGPPAR = 0;
            if (pin_type == pin_type_digital_output) {
                MCF_GPIO_DDRGP |= 1 << offset;
            } else {
                assert(pin_type == pin_type_digital_input);
                MCF_GPIO_DDRGP &= ~(1 << offset);
            }
            break;
#endif
#if MCF52233 || MCF52259 || MCF5211
        case PIN_GPT0:
        case PIN_GPT1:
        case PIN_GPT2:
        case PIN_GPT3:
            offset = pin_number - PIN_GPT0;
            assert(offset < 4);
            MCF_GPIO_PTAPAR &= ~(3<<(offset*2));
            if (pin_type == pin_type_digital_output) {
                MCF_GPIO_DDRTA |= 1 << offset;
            } else {
                assert(pin_type == pin_type_digital_input);
                MCF_GPIO_DDRTA &= ~(1 << offset);
            }
            break;
#endif
        case PIN_SCL:
        case PIN_SDA:
            offset = pin_number - PIN_SCL;
            assert(offset < 2);
            MCF_GPIO_PASPAR = 0;
            if (pin_type == pin_type_digital_output) {
                MCF_GPIO_DDRAS |= 1 << offset;
            } else {
                assert(pin_type == pin_type_digital_input);
                MCF_GPIO_DDRAS &= ~(1 << offset);
            }
            break;
        default:
            assert(0);
            break;
    }
#elif MCF51JM128
    // configure the MCF51JM128 pin for the requested function
    switch (pin_number) {
        case PIN_PTA0:
        case PIN_PTA1:
        case PIN_PTA2:
        case PIN_PTA3:
        case PIN_PTA4:
        case PIN_PTA5:
            offset = pin_number - PIN_PTA0;
            assert(offset < 6);
            if (pin_type == pin_type_digital_output) {
                PTAPE &= ~(1<<offset);
                PTADD |= 1<<offset;
            } else {
                assert(pin_type == pin_type_digital_input);
                PTAPE |= 1<<offset;
                PTADD &= ~(1<<offset);
            }
            break;
        case PIN_PTB0:  // adc0
        case PIN_PTB1:  // adc1
        case PIN_PTB2:  // adc2
        case PIN_PTB3:  // adc3
        case PIN_PTB4:  // adc4
        case PIN_PTB5:  // adc5
        case PIN_PTB6:  // adc6
        case PIN_PTB7:  // adc7
            offset = pin_number - PIN_PTB0;
            assert(offset < 8);
            adc = offset;
            if (pin_type == pin_type_analog_input) {
                assert(offset < 8);
                assert(adc < 8);
                APCTL1 |= 1<<adc;
            } else {
                if (offset < 8) {
                    assert(adc < 8);
                    APCTL1 &= ~(1<<adc);
                }
                if (pin_type == pin_type_digital_output) {
                    PTBPE &= ~(1<<offset);
                    PTBDD |= 1<<offset;
                } else {
                    assert(pin_type == pin_type_digital_input);
                    PTBPE |= 1<<offset;
                    PTBDD &= ~(1<<offset);
                }
            }
            break;
        case PIN_PTC0:
        case PIN_PTC1:
        case PIN_PTC2:
        case PIN_PTC3:  // u2tx
        case PIN_PTC4:
        case PIN_PTC5:  // u2rx
        case PIN_PTC6:
            offset = pin_number - PIN_PTC0;
            assert(offset < 8);
            if (pin_type == pin_type_uart_input) {
                assert(offset == 5);
                SCI2C2X |= SCI2C2_RE_MASK;
            } else if (pin_type == pin_type_uart_output) {
                assert(offset == 3);
                SCI2C2X |= SCI2C2_TE_MASK;
            } else {
                if (offset == 5) {
                    SCI2C2X &= ~SCI2C2_RE_MASK;
                } else if (offset == 3) {
                    SCI2C2X &= ~SCI2C2_TE_MASK;
                }
                if (pin_type == pin_type_digital_output) {
                    PTCPE &= ~(1<<offset);
                    PTCDD |= 1<<offset;
                } else {
                    assert(pin_type == pin_type_digital_input);
                    PTCPE |= 1<<offset;
                    PTCDD &= ~(1<<offset);
                }
            }
            break;
        case PIN_PTD0:  // adc8
        case PIN_PTD1:  // adc9
        case PIN_PTD2:
        case PIN_PTD3:  // adc10
        case PIN_PTD4:  // adc11
        case PIN_PTD5:
        case PIN_PTD6:
        case PIN_PTD7:
            offset = pin_number - PIN_PTD0;
            assert(offset < 8);
            adc = (offset<2)?(8+offset):(8+offset-1);
            if (pin_type == pin_type_analog_input) {
                assert(offset < 2 || offset >= 3 && offset < 5);
                assert(adc >= 8 && adc < 12);
                APCTL2 |= 1<<(adc-8);
            } else {
                if (offset < 2 || offset >= 3 && offset < 5) {
                    assert(adc >= 8 && adc < 12);
                    APCTL2 &= ~(1<<(adc-8));
                }
                if (pin_type == pin_type_digital_output) {
                    PTDPE &= ~(1<<offset);
                    PTDDD |= 1<<offset;
                } else {
                    assert(pin_type == pin_type_digital_input);
                    PTDPE |= 1<<offset;
                    PTDDD &= ~(1<<offset);
                }
            }
            break;
        case PIN_PTE0:  // u1tx
        case PIN_PTE1:  // u1rx
        case PIN_PTE2:
        case PIN_PTE3:
        case PIN_PTE4:
        case PIN_PTE5:
        case PIN_PTE6:
        case PIN_PTE7:
            offset = pin_number - PIN_PTE0;
            assert(offset < 8);
            if (pin_type == pin_type_uart_input) {
                assert(offset == 1);
                SCI1C2X |= SCI1C2_RE_MASK;
            } else if (pin_type == pin_type_uart_output) {
                assert(offset == 0);
                SCI1C2X |= SCI1C2_TE_MASK;
            } else if (pin_type == pin_type_analog_output || pin_type == pin_type_servo_output || pin_type == pin_type_frequency_output) {
                assert(offset >= 2 && offset < 4);
                n = 0;
                assert(n < LENGTHOF(freq));
                
                if (freq[n] && (pin_type == pin_type_frequency_output || freq[n] != pin_type)) {
                    printf("conflicting timer usage\n");
#if STICKOS
                    stop();
#endif
                } else {
                    if (pin_type == pin_type_frequency_output) {
                        assert(! freq[n]);

                        // set timer prescales to 16
                        if (n) {
                            TPM2SC = TPM2SC_CLKSA_MASK|TPM2SC_PS2_MASK;
                        } else {
                            TPM1SC = TPM1SC_CLKSA_MASK|TPM1SC_PS2_MASK;
                        }
                        assert(FREQ_PRESCALE == 16);
                        
                        // program our channel's frequency mode
                        if (offset == 2) {
                            TPM1C0SC = TPM1C0SC_MS0A_MASK;
                        } else {
                            TPM1C1SC = TPM1C1SC_MS1A_MASK;
                        }
                    } else {
                        if (! freq[n]) {
                            if (pin_type == pin_type_analog_output) {
                                // set pwm prescales to 1
                                mask = 0;
                                mod = pin_analog;
                            } else {
                                assert(pin_type == pin_type_servo_output);
                                
                                // set pwm prescales to 16
                                assert(SERVO_PRESCALE == 16);
                                mask = TPM1SC_PS2_MASK;
                                mod = SERVO_MOD;
                            }
                            
                            // set pwm prescales
                            TPM1SC = TPM1SC_CLKSA_MASK|mask;
                            
                            // program the counter for pwm generation (shared counter)
                            TPM1MODH = mod>>8;
                            TPM1MODL = mod&0xff;
                            TPM1CNTL = 0;
                        }
                        
                        // program our channel's pwm mode
                        if (offset == 2) {
                            TPM1C0SC = TPM1C0SC_MS0B_MASK|TPM1C0SC_ELS0B_MASK;
                        } else {
                            TPM1C1SC = TPM1C1SC_MS1B_MASK|TPM1C1SC_ELS1B_MASK;
                        }
                    }
                    
                    freq[n] = pin_type;
                }
            } else {
                // program our pin's digital mode
                if (offset == 1) {
                    SCI1C2X &= ~SCI1C2_RE_MASK;
                } else if (offset == 0) {
                    SCI1C2X &= ~SCI1C2_TE_MASK;
                } else if (offset == 2) {
                    TPM1C0SC = 0;
                } else if (offset == 3) {
                    TPM1C1SC = 0;
                }
                
                if (pin_type == pin_type_digital_output) {
                    PTEPE &= ~(1<<offset);
                    PTEDD |= 1<<offset;
                } else {
                    assert(pin_type == pin_type_digital_input);
                    PTEPE |= 1<<offset;
                    PTEDD &= ~(1<<offset);
                }
            }
            break;
        case PIN_PTF0:
        case PIN_PTF1:
        case PIN_PTF2:
        case PIN_PTF3:
        case PIN_PTF4:
        case PIN_PTF5:
        case PIN_PTF6:
        case PIN_PTF7:
            offset = pin_number - PIN_PTF0;
            assert(offset < 8);
            if (pin_type == pin_type_analog_output || pin_type == pin_type_servo_output || pin_type == pin_type_frequency_output) {
                assert(offset < 6);
                n = offset > 3;
                assert(n < LENGTHOF(freq));
                if (freq[n] && (pin_type == pin_type_frequency_output || freq[n] != pin_type)) {
                    printf("conflicting timer usage\n");
#if STICKOS
                    stop();
#endif
                } else {
                    if (pin_type == pin_type_frequency_output) {
                        assert(! freq[n]);

                        // set timer prescales to 16
                        if (n) {
                            TPM2SC = TPM2SC_CLKSA_MASK|TPM2SC_PS2_MASK;
                        } else {
                            TPM1SC = TPM1SC_CLKSA_MASK|TPM1SC_PS2_MASK;
                        }
                        assert(FREQ_PRESCALE == 16);
                        // program our channel's frequency mode
                        if (offset == 0) {
                            TPM1C2SC = TPM1C2SC_MS2A_MASK;
                        } else if (offset == 1) {
                            TPM1C3SC = TPM1C3SC_MS3A_MASK;
                        } else if (offset == 2) {
                            TPM1C4SC = TPM1C4SC_MS4A_MASK;
                        } else if (offset == 3) {
                            TPM1C5SC = TPM1C5SC_MS5A_MASK;
                        } else if (offset == 4) {
                            TPM2C0SC = TPM2C0SC_MS0A_MASK;
                        } else {
                            TPM2C1SC = TPM2C1SC_MS1A_MASK;
                        }
                    } else {
                        if (! freq[n]) {
                            if (pin_type == pin_type_analog_output) {
                                // set pwm prescales to 1
                                mask = 0;
                                mod = pin_analog;
                            } else {
                                assert(pin_type == pin_type_servo_output);
                                
                                // set pwm prescales to 16
                                assert(SERVO_PRESCALE == 16);
                                mask = TPM1SC_PS2_MASK;
                                mod = SERVO_MOD;
                            }

                            // set pwm prescales
                            if (n) {
                                TPM2SC = TPM1SC_CLKSA_MASK|mask;
                            } else {
                                TPM1SC = TPM1SC_CLKSA_MASK|mask;
                            }
                            
                            // program the counter for pwm generation (shared counter)
                            if (n) {
                                TPM2MODH = mod>>8;
                                TPM2MODL = mod&0xff;
                                TPM2CNTL = 0;
                            } else {
                                TPM1MODH = mod>>8;
                                TPM1MODL = mod&0xff;
                                TPM1CNTL = 0;
                            }
                        }
                        
                        // program our channel's pwm mode
                        if (offset == 0) {
                            TPM1C2SC = TPM1C2SC_MS2B_MASK|TPM1C2SC_ELS2B_MASK;
                        } else if (offset == 1) {
                            TPM1C3SC = TPM1C3SC_MS3B_MASK|TPM1C3SC_ELS3B_MASK;
                        } else if (offset == 2) {
                            TPM1C4SC = TPM1C4SC_MS4B_MASK|TPM1C4SC_ELS4B_MASK;
                        } else if (offset == 3) {
                            TPM1C5SC = TPM1C5SC_MS5B_MASK|TPM1C5SC_ELS5B_MASK;
                        } else if (offset == 4) {
                            TPM2C0SC = TPM2C0SC_MS0B_MASK|TPM2C0SC_ELS0B_MASK;
                        } else {
                            TPM2C1SC = TPM2C1SC_MS1B_MASK|TPM2C1SC_ELS1B_MASK;
                        }
                    }
                    
                    freq[n] = pin_type;
                }
            } else {
                // program our pin's digital mode
                if (offset == 0) {
                    TPM1C2SC = 0;
                } else if (offset == 1) {
                    TPM1C3SC = 0;
                } else if (offset == 2) {
                    TPM1C4SC = 0;
                } else if (offset == 3) {
                    TPM1C5SC = 0;
                } else if (offset == 4) {
                    TPM2C0SC = 0;
                } else if (offset == 5) {
                    TPM2C1SC = 0;
                }
                
                if (pin_type == pin_type_digital_output) {
                    PTFPE &= ~(1<<offset);
                    PTFDD |= 1<<offset;
                } else {
                    assert(pin_type == pin_type_digital_input);
                    PTFPE |= 1<<offset;
                    PTFDD &= ~(1<<offset);
                }
            }
            break;
        case PIN_PTG0:
        case PIN_PTG1:
        case PIN_PTG2:
        case PIN_PTG3:
            offset = pin_number - PIN_PTG0;
            assert(offset < 4);
            if (pin_type == pin_type_digital_output) {
                PTGPE &= ~(1<<offset);
                PTGDD |= 1<<offset;
            } else {
                assert(pin_type == pin_type_digital_input);
                PTGPE |= 1<<offset;
                PTGDD &= ~(1<<offset);
            }
            break;
        default:
            assert(0);
            break;
    }
#elif MCF51CN128
#define SETMUX(r, o, m)  assert(o < 8); \
                         if (o >= 4) { \
                            PT ## r ## PF1 = (PT ## r ## PF1 &~ (3<<((o-4)*2))) | (m<<((o-4)*2)); \
                         } else { \
                            PT ## r ## PF2 = (PT ## r ## PF2 &~ (3<<((o)*2))) | (m<<((o)*2)); \
                         }
    // configure the MCF51CN128 pin for the requested function
    switch (pin_number) {
        case PIN_PTA0:
        case PIN_PTA1:
        case PIN_PTA2:
        case PIN_PTA3:  // u3tx
        case PIN_PTA4:  // u3rx
        case PIN_PTA5:
        case PIN_PTA6:
        case PIN_PTA7:
            offset = pin_number - PIN_PTA0;
            assert(offset < 8);
            if (pin_type == pin_type_uart_input) {
                assert(offset == 4);
                SCI3C2 |= SCI3C2_RE_MASK;
                SETMUX(A, offset, 2);
            } else if (pin_type == pin_type_uart_output) {
                assert(offset == 3);
                SCI3C2 |= SCI3C2_TE_MASK;
                SETMUX(A, offset, 2);
            } else {
                // program our pin's digital mode
                if (offset == 4) {
                    SCI3C2 &= ~SCI3C2_RE_MASK;
                } else if (offset == 3) {
                    SCI3C2 &= ~SCI3C2_TE_MASK;
                }

                if (pin_type == pin_type_digital_output) {
                    PTAPE &= ~(1<<offset);
                    PTADD |= 1<<offset;
                } else {
                    assert(pin_type == pin_type_digital_input);
                    PTAPE |= 1<<offset;
                    PTADD &= ~(1<<offset);
                }
                SETMUX(A, offset, 0);
            }
            break;
        case PIN_PTB0:
        case PIN_PTB1:
        case PIN_PTB2:
        case PIN_PTB3:
        case PIN_PTB4:
        case PIN_PTB5:
        case PIN_PTB6:  // tpm2ch0
        case PIN_PTB7:  // tpm2ch1
            offset = pin_number - PIN_PTB0;
            assert(offset < 8);
            if (pin_type == pin_type_analog_output || pin_type == pin_type_servo_output || pin_type == pin_type_frequency_output) {
                assert(offset >= 6 && offset < 8);
                n = 1;
                assert(n < LENGTHOF(freq));
                if (freq[n] && (pin_type == pin_type_frequency_output || freq[n] != pin_type)) {
                    printf("conflicting timer usage\n");
#if STICKOS
                    stop();
#endif
                } else {
                    if (pin_type == pin_type_frequency_output) {
                        assert(! freq[n]);

                        // set timer prescales to 16
                        assert(n);
                        TPM2SC = TPM2SC_CLKSA_MASK|TPM2SC_PS2_MASK;
                        assert(FREQ_PRESCALE == 16);
                        // program our channel's frequency mode
                        if (offset == 6) {
                            TPM2C0SC = TPM2C0SC_MS0A_MASK;
                        } else {
                            assert(offset == 7);
                            TPM2C1SC = TPM2C1SC_MS1A_MASK;
                        }
                    } else {
                        if (! freq[n]) {
                            if (pin_type == pin_type_analog_output) {
                                // set pwm prescales to 1
                                mask = 0;
                                mod = pin_analog;
                            } else {
                                assert(pin_type == pin_type_servo_output);
                                
                                // set pwm prescales to 16
                                assert(SERVO_PRESCALE == 16);
                                mask = TPM2SC_PS2_MASK;
                                mod = SERVO_MOD;
                            }
                        
                            // set pwm prescales
                            assert(n);
                            TPM2SC = TPM2SC_CLKSA_MASK|mask;
                            
                            // program the counter for pwm generation (shared counter)
                            assert(n);
                            TPM2MODH = mod>>8;
                            TPM2MODL = mod&0xff;
                            TPM2CNTL = 0;
                        }
                        
                        // program our channel's pwm mode
                        if (offset == 6) {
                            TPM2C0SC = TPM2C0SC_MS0B_MASK|TPM2C0SC_ELS0B_MASK;
                        } else {
                            assert(offset == 7);
                            TPM2C1SC = TPM2C1SC_MS1B_MASK|TPM2C1SC_ELS1B_MASK;
                        }
                    }
                    
                    freq[n] = pin_type;
                }
                SETMUX(B, offset, 3);
            } else {
                // program our pin's digital mode
                if (offset == 6) {
                    TPM2C0SC = 0;
                } else if (offset == 7) {
                    TPM2C1SC = 0;
                }

                if (pin_type == pin_type_digital_output) {
                    PTBPE &= ~(1<<offset);
                    PTBDD |= 1<<offset;
                } else {
                    assert(pin_type == pin_type_digital_input);
                    PTBPE |= 1<<offset;
                    PTBDD &= ~(1<<offset);
                }
                SETMUX(B, offset, 0);
            }
            break;
        case PIN_PTC0:  // tpm2ch2
        case PIN_PTC1:
        case PIN_PTC2:
        case PIN_PTC3:
        case PIN_PTC4:  // adc11
        case PIN_PTC5:  // adc10
        case PIN_PTC6:  // adc9
        case PIN_PTC7:  // adc8
            offset = pin_number - PIN_PTC0;
            assert(offset < 8);
            if (pin_type == pin_type_analog_input) {
                assert(offset >= 4 && offset < 8);
                SETMUX(C, offset, 3);
            } else if (pin_type == pin_type_analog_output || pin_type == pin_type_servo_output || pin_type == pin_type_frequency_output) {
                assert(offset == 0);
                n = 1;
                assert(n < LENGTHOF(freq));
                if (freq[n] && (pin_type == pin_type_frequency_output || freq[n] != pin_type)) {
                    printf("conflicting timer usage\n");
#if STICKOS
                    stop();
#endif
                } else {
                    if (pin_type == pin_type_frequency_output) {
                        assert(! freq[n]);

                        // set timer prescales to 16
                        assert(n);
                        TPM2SC = TPM2SC_CLKSA_MASK|TPM2SC_PS2_MASK;
                        assert(FREQ_PRESCALE == 16);
                        // program our channel's frequency mode
                        assert(offset == 0);
                        TPM2C2SC = TPM2C2SC_MS2A_MASK;
                    } else {
                        if (! freq[n]) {
                            if (pin_type == pin_type_analog_output) {
                                // set pwm prescales to 1
                                mask = 0;
                                mod = pin_analog;
                            } else {
                                assert(pin_type == pin_type_servo_output);
                                
                                // set pwm prescales to 16
                                assert(SERVO_PRESCALE == 16);
                                mask = TPM2SC_PS2_MASK;
                                mod = SERVO_MOD;
                            }
                            
                            // set pwm prescales
                            assert(n);
                            TPM2SC = TPM2SC_CLKSA_MASK|mask;
                            
                            // program the counter for pwm generation (shared counter)
                            assert(n);
                            TPM2MODH = mod>>8;
                            TPM2MODL = mod&0xff;
                            TPM2CNTL = 0;
                        }
                        
                        // program our channel's pwm mode
                        assert(offset == 0);
                        TPM2C2SC = TPM2C2SC_MS2B_MASK|TPM2C2SC_ELS2B_MASK;
                    }
                    
                    freq[n] = pin_type;
                }
                SETMUX(C, offset, 3);
            } else {
                // program our pin's digital mode
                if (offset == 0) {
                    TPM2C2SC = 0;
                }

                if (pin_type == pin_type_digital_output) {
                    PTCPE &= ~(1<<offset);
                    PTCDD |= 1<<offset;
                } else {
                    assert(pin_type == pin_type_digital_input);
                    PTCPE |= 1<<offset;
                    PTCDD &= ~(1<<offset);
                }
                SETMUX(C, offset, 0);
            }
            break;
        case PIN_PTD0:  // adc7, u1tx
        case PIN_PTD1:  // adc6, u1rx
        case PIN_PTD2:  // adc5, u2tx
        case PIN_PTD3:  // adc4, u2rx
        case PIN_PTD4:
        case PIN_PTD5:
        case PIN_PTD6:
        case PIN_PTD7:  // adc3
            offset = pin_number - PIN_PTD0;
            assert(offset < 8);
            if (pin_type == pin_type_analog_input) {
                assert(offset < 4 || offset == 7);
                SETMUX(D, offset, 3);
            } else if (pin_type == pin_type_uart_input) {
                assert(offset == 1 || offset == 3);
                if (offset == 1) {
                    SCI1C2 |= SCI1C2_RE_MASK;
                } else {
                    SCI2C2 |= SCI2C2_RE_MASK;
                }
                SETMUX(D, offset, 2);
            } else if (pin_type == pin_type_uart_output) {
                assert(offset == 0 || offset == 2);
                if (offset == 0) {
                    SCI1C2 |= SCI1C2_TE_MASK;
                } else {
                    SCI2C2 |= SCI2C2_TE_MASK;
                }
                SETMUX(D, offset, 2);
            } else {
                // program our pin's digital mode
                if (offset == 0) {
                    SCI1C2 &= ~SCI1C2_TE_MASK;
                } else if (offset == 1) {
                    SCI1C2 &= ~SCI1C2_RE_MASK;
                } else if (offset == 2) {
                    SCI2C2 &= ~SCI2C2_TE_MASK;
                } else if (offset == 3) {
                    SCI2C2 &= ~SCI3C2_RE_MASK;
                }

                if (pin_type == pin_type_digital_output) {
                    PTDPE &= ~(1<<offset);
                    PTDDD |= 1<<offset;
                } else {
                    assert(pin_type == pin_type_digital_input);
                    PTDPE |= 1<<offset;
                    PTDDD &= ~(1<<offset);
                }
                SETMUX(D, offset, 0);
            }
            break;
        case PIN_PTE0:  // adc2
        case PIN_PTE1:  // adc1
        case PIN_PTE2:  // adc0
        case PIN_PTE3:  // tpm1ch0
        case PIN_PTE4:  // tpm1ch1
        case PIN_PTE5:  // tpm1ch2
        case PIN_PTE6:
        case PIN_PTE7:
            offset = pin_number - PIN_PTE0;
            assert(offset < 8);
            if (pin_type == pin_type_analog_input) {
                assert(offset < 3);
                SETMUX(E, offset, 3);
            } else if (pin_type == pin_type_analog_output || pin_type == pin_type_servo_output || pin_type == pin_type_frequency_output) {
                assert(offset >= 3 && offset < 6);
                n = 0;
                assert(n < LENGTHOF(freq));
                if (freq[n] && (pin_type == pin_type_frequency_output || freq[n] != pin_type)) {
                    printf("conflicting timer usage\n");
#if STICKOS
                    stop();
#endif
                } else {
                    if (pin_type == pin_type_frequency_output) {
                        assert(! freq[n]);

                        // set timer prescales to 16
                        assert(! n);
                        TPM1SC = TPM1SC_CLKSA_MASK|TPM1SC_PS2_MASK;
                        assert(FREQ_PRESCALE == 16);
                        // program our channel's frequency mode
                        if (offset == 3) {
                            TPM1C0SC = TPM1C0SC_MS0A_MASK;
                        } else if (offset == 4) {
                            TPM1C1SC = TPM1C1SC_MS1A_MASK;
                        } else {
                            assert(offset == 5);
                            TPM1C2SC = TPM1C2SC_MS2A_MASK;
                        }
                    } else {
                        if (! freq[n]) {
                            if (pin_type == pin_type_analog_output) {
                                // set pwm prescales to 1
                                mask = 0;
                                mod = pin_analog;
                            } else {
                                assert(pin_type == pin_type_servo_output);
                                
                                // set pwm prescales to 16
                                assert(SERVO_PRESCALE == 16);
                                mask = TPM1SC_PS2_MASK;
                                mod = SERVO_MOD;
                            }
                        
                            // set pwm prescales
                            assert(! n);
                            TPM1SC = TPM1SC_CLKSA_MASK|mask;
                            
                            // program the counter for pwm generation (shared counter)
                            assert(! n);
                            TPM1MODH = mod>>8;
                            TPM1MODL = mod&0xff;
                            TPM1CNTL = 0;
                        }
                        
                        // program our channel's pwm mode
                        if (offset == 3) {
                            TPM1C0SC = TPM1C0SC_MS0B_MASK|TPM1C0SC_ELS0B_MASK;
                        } else if (offset == 4) {
                            TPM1C1SC = TPM1C1SC_MS1B_MASK|TPM1C1SC_ELS1B_MASK;
                        } else {
                            assert(offset == 5);
                            TPM1C2SC = TPM1C2SC_MS2B_MASK|TPM1C2SC_ELS2B_MASK;
                        }
                    }
                    
                    freq[n] = pin_type;
                }
                SETMUX(E, offset, 3);
            }else {                
                // program our pin's digital mode
                if (offset == 3) {
                    TPM1C0SC = 0;
                } else if (offset == 4) {
                    TPM1C1SC = 0;
                } else if (offset == 5) {
                    TPM1C2SC = 0;
                }

                if (pin_type == pin_type_digital_output) {
                    PTEPE &= ~(1<<offset);
                    PTEDD |= 1<<offset;
                } else {
                    assert(pin_type == pin_type_digital_input);
                    PTEPE |= 1<<offset;
                    PTEDD &= ~(1<<offset);
                }
                SETMUX(E, offset, 0);
            }
            break;
        case PIN_PTF0:
        case PIN_PTF1:
        case PIN_PTF2:
        case PIN_PTF3:
        case PIN_PTF4:
        case PIN_PTF5:
        case PIN_PTF6:
        case PIN_PTF7:
            offset = pin_number - PIN_PTF0;
            assert(offset < 8);
            if (pin_type == pin_type_digital_output) {
                PTFPE &= ~(1<<offset);
                PTFDD |= 1<<offset;
            } else {
                assert(pin_type == pin_type_digital_input);
                PTFPE |= 1<<offset;
                PTFDD &= ~(1<<offset);
            }
            break;
        case PIN_PTG0:
        case PIN_PTG1:
        case PIN_PTG2:
        case PIN_PTG3:
        case PIN_PTG4:
        case PIN_PTG5:
        case PIN_PTG6:
        case PIN_PTG7:
            offset = pin_number - PIN_PTG0;
            assert(offset < 8);
            if (pin_type == pin_type_digital_output) {
                PTGPE &= ~(1<<offset);
                PTGDD |= 1<<offset;
            } else {
                assert(pin_type == pin_type_digital_input);
                PTGPE |= 1<<offset;
                PTGDD &= ~(1<<offset);
            }
            break;
        case PIN_PTH0:
        case PIN_PTH1:
        case PIN_PTH2:
        case PIN_PTH3:
        case PIN_PTH4:
        case PIN_PTH5:
        case PIN_PTH6:
        case PIN_PTH7:
            offset = pin_number - PIN_PTH0;
            assert(offset < 8);
            if (pin_type == pin_type_digital_output) {
                PTHPE &= ~(1<<offset);
                PTHDD |= 1<<offset;
            } else {
                assert(pin_type == pin_type_digital_input);
                PTHPE |= 1<<offset;
                PTHDD &= ~(1<<offset);
            }
            break;
        case PIN_PTJ0:
        case PIN_PTJ1:
        case PIN_PTJ2:
        case PIN_PTJ3:
        case PIN_PTJ4:
        case PIN_PTJ5:
            offset = pin_number - PIN_PTJ0;
            assert(offset < 6);
            if (pin_type == pin_type_digital_output) {
                PTJPE &= ~(1<<offset);
                PTJDD |= 1<<offset;
            } else {
                assert(pin_type == pin_type_digital_input);
                PTJPE |= 1<<offset;
                PTJDD &= ~(1<<offset);
            }
            break;
        default:
            assert(0);
            break;
    }
#elif MCF51QE128 || MC9S08QE128
    // configure the MCF51QE128/MC9S08QE128 pin for the requested function
    switch (pin_number) {
        case PIN_PTA0:  // adc0, tpm1ch0
        case PIN_PTA1:  // adc1, tpm2ch0
        case PIN_PTA2:  // adc2
        case PIN_PTA3:  // adc3
        case PIN_PTA4:
        case PIN_PTA5:
        case PIN_PTA6:  // adc8, tpm1ch2
        case PIN_PTA7:  // adc9, tpm2ch2
            offset = pin_number - PIN_PTA0;
            assert(offset < 8);
            adc = (offset<4)?(offset):(8+offset-6);
            if (pin_type == pin_type_analog_input) {
                if (offset < 4) {
                    assert(adc < 4);
                    APCTL1 |= 1<<adc;
                } else {
                    assert(offset >= 6);
                    assert(adc >= 8 && adc < 10);
                    APCTL2 |= 1<<(adc-8);
                }
            } else {
                if (offset < 4) {
                    assert(adc < 4);
                    APCTL1 &= ~(1<<adc);
                } else if (offset >= 6) {
                    assert(adc >= 8 && adc < 10);
                    APCTL2 &= ~(1<<(adc-8));
                }
                
                if (pin_type == pin_type_analog_output || pin_type == pin_type_servo_output || pin_type == pin_type_frequency_output) {
                    assert(offset < 2 || offset >= 6);
                    n = offset&1;
                    assert(n < LENGTHOF(freq));
                    if (freq[n] && (pin_type == pin_type_frequency_output || freq[n] != pin_type)) {
                        printf("conflicting timer usage\n");
#if STICKOS
                        stop();
#endif
                    } else {
                        if (pin_type == pin_type_frequency_output) {
                            assert(! freq[n]);

                            // set timer prescales to 16
                            if (n) {
                                TPM2SC = TPM2SC_CLKSA_MASK|TPM2SC_PS2_MASK;
                            } else {
                                TPM1SC = TPM1SC_CLKSA_MASK|TPM1SC_PS2_MASK;
                            }
                            assert(FREQ_PRESCALE == 16);
                            // program our channel's frequency mode
                            if (offset == 0) {
                                TPM1C0SC = TPM1C0SC_MS0A_MASK;
                            } else if (offset == 1) {
                                TPM2C0SC = TPM2C0SC_MS0A_MASK;
                            } else if (offset == 6) {
                                TPM1C2SC = TPM1C2SC_MS2A_MASK;
                            } else {
                                assert(offset == 7);
                                TPM2C2SC = TPM2C2SC_MS2A_MASK;
                            }
                        } else {
                            if (! freq[n]) {
                                if (pin_type == pin_type_analog_output) {
                                    // set pwm prescales to 1
                                    mask = 0;
                                    mod = pin_analog;
                                } else {
                                    assert(pin_type == pin_type_servo_output);
                                    
                                    // set pwm prescales to 16
                                    assert(SERVO_PRESCALE == 16);
                                    mask = TPM1SC_PS2_MASK;
                                    mod = SERVO_MOD;
                                }
                            
                                // set pwm prescales
                                if (n) {
                                    TPM2SC = TPM2SC_CLKSA_MASK|mask;
                                } else {
                                    TPM1SC = TPM1SC_CLKSA_MASK|mask;
                                }
                                
                                // program the counter for pwm generation (shared counter)
                                if (n) {
                                    TPM2MODH = mod>>8;
                                    TPM2MODL = mod&0xff;
                                    TPM2CNTL = 0;
                                } else {
                                    TPM1MODH = mod>>8;
                                    TPM1MODL = mod&0xff;
                                    TPM1CNTL = 0;
                                }
                            }
                            
                            // program our channel's pwm mode
                            if (offset == 0) {
                                TPM1C0SC = TPM1C0SC_MS0B_MASK|TPM1C0SC_ELS0B_MASK;
                            } else if (offset == 1) {
                                TPM2C0SC = TPM2C0SC_MS0B_MASK|TPM2C0SC_ELS0B_MASK;
                            } else if (offset == 6) {
                                TPM1C2SC = TPM1C2SC_MS2B_MASK|TPM1C2SC_ELS2B_MASK;
                            } else {
                                assert(offset == 7);
                                TPM2C2SC = TPM2C2SC_MS2B_MASK|TPM2C2SC_ELS2B_MASK;
                            }
                        }
                        
                        freq[n] = pin_type;
                    }
                } else {
                    // program our pin's digital mode
                    if (offset == 0) {
                        TPM1C0SC = 0;
                    } else if (offset == 1) {
                        TPM2C0SC = 0;
                    } else if (offset == 6) {
                        TPM1C2SC = 0;
                    } else if (offset == 7) {
                        TPM2C2SC = 0;
                    }
                    
                    if (pin_type == pin_type_digital_output) {
                        PTAPE &= ~(1<<offset);
                        PTADD |= 1<<offset;
                    } else {
                        assert(pin_type == pin_type_digital_input);
                        PTAPE |= 1<<offset;
                        PTADD &= ~(1<<offset);
                    }
                }
            }
            break;
        case PIN_PTB0:  // adc4, u1rx
        case PIN_PTB1:  // adc5, u1tx
        case PIN_PTB2:  // adc6
        case PIN_PTB3:  // adc7
        case PIN_PTB4:  // tpm2ch1
        case PIN_PTB5:  // tpm1ch1
        case PIN_PTB6:
        case PIN_PTB7:
            offset = pin_number - PIN_PTB0;
            assert(offset < 8);
            adc = 4+offset;
            if (pin_type == pin_type_analog_input) {
                assert(offset >= 0 && offset < 4);
                assert(adc >= 4 && adc < 8);
                APCTL1 |= 1<<adc;
            } else {
                if (offset >= 0 && offset < 4) {
                    assert(adc >= 4 && adc < 8);                
                    APCTL1 &= ~(1<<adc);
                }
                if (pin_type == pin_type_uart_input) {
                    assert(offset == 0);
                    SCI1C2X |= SCI1C2_RE_MASK;
                } else if (pin_type == pin_type_uart_output) {
                    assert(offset == 1);
                    SCI1C2X |= SCI1C2_TE_MASK;
                } else {
                    if (offset == 0) {
                        SCI1C2X &= ~SCI1C2_RE_MASK;
                    } else if (offset == 1) {
                        SCI1C2X &= ~SCI1C2_TE_MASK;
                    }
                    
                    if (pin_type == pin_type_analog_output || pin_type == pin_type_servo_output || pin_type == pin_type_frequency_output) {
                        assert(offset >= 4 && offset < 6);
                        n = offset == 4;
                        assert(n < LENGTHOF(freq));
                        if (freq[n] && (pin_type == pin_type_frequency_output || freq[n] != pin_type)) {
                            printf("conflicting timer usage\n");
#if STICKOS
                            stop();
#endif
                        } else {
                            if (pin_type == pin_type_frequency_output) {
                                assert(! freq[n]);

                                // set timer prescales to 16
                                if (n) {
                                    TPM2SC = TPM2SC_CLKSA_MASK|TPM2SC_PS2_MASK;
                                } else {
                                    TPM1SC = TPM1SC_CLKSA_MASK|TPM1SC_PS2_MASK;
                                }
                                assert(FREQ_PRESCALE == 16);
                                // program our channel's frequency mode
                                if (offset == 5) {
                                    TPM1C1SC = TPM1C1SC_MS1A_MASK;
                                } else {
                                    assert(offset == 4);
                                    TPM2C1SC = TPM2C1SC_MS1A_MASK;
                                }
                            } else {
                                if (! freq[n]) {
                                    if (pin_type == pin_type_analog_output) {
                                        // set pwm prescales to 1
                                        mask = 0;
                                        mod = pin_analog;
                                    } else {
                                        assert(pin_type == pin_type_servo_output);
                                        
                                        // set pwm prescales to 16
                                        assert(SERVO_PRESCALE == 16);
                                        mask = TPM1SC_PS2_MASK;
                                        mod = SERVO_MOD;
                                    }
                                
                                    // set pwm prescales
                                    if (n) {
                                        TPM2SC = TPM1SC_CLKSA_MASK|mask;
                                    } else {
                                        TPM1SC = TPM1SC_CLKSA_MASK|mask;
                                    }
                                    
                                    // program the counter for pwm generation (shared counter)
                                    if (n) {
                                        TPM2MODH = mod>>8;
                                        TPM2MODL = mod&0xff;
                                        TPM2CNTL = 0;
                                    } else {
                                        TPM1MODH = mod>>8;
                                        TPM1MODL = mod&0xff;
                                        TPM1CNTL = 0;
                                    }
                                }
                                
                                // program our channel's pwm mode
                                if (offset == 5) {
                                    TPM1C1SC = TPM1C1SC_MS1B_MASK|TPM1C1SC_ELS1B_MASK;
                                } else {
                                    assert(offset == 4);
                                    TPM2C1SC = TPM2C1SC_MS1B_MASK|TPM2C1SC_ELS1B_MASK;
                                }
                            }
                            
                            freq[n] = pin_type;
                        }
                    } else {
                        // program our pin's digital mode
                        if (offset == 5) {
                            TPM1C1SC = 0;
                        } else if (offset == 4) {
                            TPM2C1SC = 0;
                        }

                        if (pin_type == pin_type_digital_output) {
                            PTBPE &= ~(1<<offset);
                            PTBDD |= 1<<offset;
                        } else {
                            assert(pin_type == pin_type_digital_input);
                            PTBPE |= 1<<offset;
                            PTBDD &= ~(1<<offset);
                        }
                    }
                }
            }
            break;
        case PIN_PTC0:  // tpm3ch0
        case PIN_PTC1:  // tpm3ch1
        case PIN_PTC2:  // tpm3ch2
        case PIN_PTC3:  // tpm3ch3
        case PIN_PTC4:  // tpm3ch4
        case PIN_PTC5:  // tpm3ch5
        case PIN_PTC6:  // u2rx
        case PIN_PTC7:  // u2tx
            offset = pin_number - PIN_PTC0;
            assert(offset < 8);
            if (pin_type == pin_type_uart_input) {
                assert(offset == 6);
                SCI2C2X |= SCI2C2_RE_MASK;
            } else if (pin_type == pin_type_uart_output) {
                assert(offset == 7);
                SCI2C2X |= SCI2C2_TE_MASK;
            } else {
                if (offset == 6) {
                    SCI2C2X &= ~SCI2C2_RE_MASK;
                } else if (offset == 7) {
                    SCI2C2X &= ~SCI2C2_TE_MASK;
                }

                if (pin_type == pin_type_analog_output || pin_type == pin_type_servo_output || pin_type == pin_type_frequency_output) {
                    assert(offset < 6);
                    n = 2;
                    assert(n < LENGTHOF(freq));
                    if (freq[n] && (pin_type == pin_type_frequency_output || freq[n] != pin_type)) {
                        printf("conflicting timer usage\n");
#if STICKOS
                        stop();
#endif
                    } else {
                        if (pin_type == pin_type_frequency_output) {
                            assert(! freq[n]);

                            // set timer prescales to 16
                            TPM3SC = TPM3SC_CLKSA_MASK|TPM3SC_PS2_MASK;
                            assert(FREQ_PRESCALE == 16);
                            // program our channel's frequency mode
                            if (offset == 0) {
                                TPM3C0SC = TPM3C0SC_MS0A_MASK;
                            } else if (offset == 1) {
                                TPM3C1SC = TPM3C1SC_MS1A_MASK;
                            } else if (offset == 2) {
                                TPM3C2SC = TPM3C2SC_MS2A_MASK;
                            } else if (offset == 3) {
                                TPM3C3SC = TPM3C3SC_MS3A_MASK;
                            } else if (offset == 4) {
                                TPM3C4SC = TPM3C4SC_MS4A_MASK;
                            } else {
                                assert(offset == 5);
                                TPM3C5SC = TPM3C5SC_MS5A_MASK;
                            }
                        } else {
                            if (! freq[n]) {
                                if (pin_type == pin_type_analog_output) {
                                    // set pwm prescales to 1
                                    mask = 0;
                                    mod = pin_analog;
                                } else {
                                    assert(pin_type == pin_type_servo_output);
                                    
                                    // set pwm prescales to 16
                                    assert(SERVO_PRESCALE == 16);
                                    mask = TPM1SC_PS2_MASK;
                                    mod = SERVO_MOD;
                                }
                                
                                // set pwm prescales
                                TPM3SC = TPM3SC_CLKSA_MASK|mask;
                                
                                // program the counter for pwm generation (shared counter)
                                TPM3MODH = mod>>8;
                                TPM3MODL = mod&0xff;
                                TPM3CNTL = 0;
                            }
                            
                            // program our channel's pwm mode
                            if (offset == 0) {
                                TPM3C0SC = TPM3C0SC_MS0B_MASK|TPM3C0SC_ELS0B_MASK;
                            } else if (offset == 1) {
                                TPM3C1SC = TPM3C1SC_MS1B_MASK|TPM3C1SC_ELS1B_MASK;
                            } else if (offset == 2) {
                                TPM3C2SC = TPM3C2SC_MS2B_MASK|TPM3C2SC_ELS2B_MASK;
                            } else if (offset == 3) {
                                TPM3C3SC = TPM3C3SC_MS3B_MASK|TPM3C3SC_ELS3B_MASK;
                            } else if (offset == 4) {
                                TPM3C4SC = TPM3C4SC_MS4B_MASK|TPM3C4SC_ELS4B_MASK;
                            } else {
                                assert(offset == 5);
                                TPM3C5SC = TPM3C5SC_MS5B_MASK|TPM3C5SC_ELS5B_MASK;
                            }
                        }
                        
                        freq[n] = pin_type;
                    }
                } else {
                    // program our pin's digital mode
                    if (offset == 0) {
                        TPM3C0SC = 0;
                    } else if (offset == 1) {
                        TPM3C1SC = 0;                        
                    } else if (offset == 2) {
                        TPM3C2SC = 0;                        
                    } else if (offset == 3) {
                        TPM3C3SC = 0;                        
                    } else if (offset == 4) {
                        TPM3C4SC = 0;                        
                    } else if (offset == 5) {
                        TPM3C5SC = 0;
                    }

                    if (pin_type == pin_type_digital_output) {
                        PTCPE &= ~(1<<offset);
                        PTCDD |= 1<<offset;
                    } else {
                        assert(pin_type == pin_type_digital_input);
                        PTCPE |= 1<<offset;
                        PTCDD &= ~(1<<offset);
                    }
                }
            }
            break;
        case PIN_PTD0:
        case PIN_PTD1:
        case PIN_PTD2:
        case PIN_PTD3:
        case PIN_PTD4:
        case PIN_PTD5:
        case PIN_PTD6:
        case PIN_PTD7:
            offset = pin_number - PIN_PTD0;
            assert(offset < 8);
            if (pin_type == pin_type_digital_output) {
                PTDPE &= ~(1<<offset);
                PTDDD |= 1<<offset;
            } else {
                assert(pin_type == pin_type_digital_input);
                PTDPE |= 1<<offset;
                PTDDD &= ~(1<<offset);
            }
            break;
        case PIN_PTE0:
        case PIN_PTE1:
        case PIN_PTE2:
        case PIN_PTE3:
        case PIN_PTE4:
        case PIN_PTE5:
        case PIN_PTE6:
        case PIN_PTE7:
            offset = pin_number - PIN_PTE0;
            assert(offset < 8);
            if (pin_type == pin_type_digital_output) {
                PTEPE &= ~(1<<offset);
                PTEDD |= 1<<offset;
            } else {
                assert(pin_type == pin_type_digital_input);
                PTEPE |= 1<<offset;
                PTEDD &= ~(1<<offset);
            }
            break;
        case PIN_PTF0:  // adc10
        case PIN_PTF1:  // adc11
        case PIN_PTF2:  // adc12
        case PIN_PTF3:  // adc13
        case PIN_PTF4:  // adc14
        case PIN_PTF5:  // adc15
        case PIN_PTF6:  // adc16
        case PIN_PTF7:  // adc17
            offset = pin_number - PIN_PTF0;
            assert(offset < 8);
            adc = 10+offset;
            if (pin_type == pin_type_analog_input) {
                assert(offset >= 0 && offset < 8);
                if (offset < 6) {
                    assert(adc >= 10 && adc < 16);
                    APCTL2 |= 1<<(adc-8);
                } else {
                    assert(offset >= 6);
                    assert(adc >= 16 && adc < 18);
                    APCTL3 |= 1<<(adc-16);
                }
            } else {
                if (offset < 6) {
                    assert(adc >= 10 && adc < 16);
                    APCTL2 &= ~(1<<(adc-8));
                } else {
                    assert(adc >= 16 && adc < 18);
                    APCTL3 &= ~(1<<(adc-16));
                }
                if (pin_type == pin_type_digital_output) {
                    PTFPE &= ~(1<<offset);
                    PTFDD |= 1<<offset;
                } else {
                    assert(pin_type == pin_type_digital_input);
                    PTFPE |= 1<<offset;
                    PTFDD &= ~(1<<offset);
                }
            }
            break;
        case PIN_PTG0:
        case PIN_PTG1:
        case PIN_PTG2:  // adc18
        case PIN_PTG3:  // adc19
            offset = pin_number - PIN_PTG0;
            assert(offset < 4);
            adc = 16+offset;
            if (pin_type == pin_type_analog_input) {
                assert(offset >= 2);
                assert(adc >= 18 && adc < 20);
                APCTL3 |= 1<<(adc-16);
            } else {
                if (offset >= 2) {
                    assert(adc >= 18 && adc < 20);
                    APCTL3 &= ~(1<<(adc-16));
                }
                if (pin_type == pin_type_digital_output) {
                    PTGPE &= ~(1<<offset);
                    PTGDD |= 1<<offset;
                } else {
                    assert(pin_type == pin_type_digital_input);
                    PTGPE |= 1<<offset;
                    PTGDD &= ~(1<<offset);
                }
            }
            break;
        default:
            assert(0);
            break;
    }
#elif MC9S12DT256 || MC9S12DP512
    // configure the MC9S12DT256/MC9S12DP512 pin for the requested function
    switch (pin_number) {
        case PIN_PAD00:
        case PIN_PAD01:
        case PIN_PAD02:
        case PIN_PAD03:
        case PIN_PAD04:
        case PIN_PAD05:
        case PIN_PAD06:
        case PIN_PAD07:
            offset = pin_number - PIN_PAD00;
            assert(offset < 8);
            adc = offset;
            if (pin_type == pin_type_analog_input) {
                ATD0DIEN &= ~(1<<offset);
            } else {
                assert(pin_type == pin_type_digital_input);
                ATD0DIEN |= 1<<offset;
            }
            break;
        case PIN_PAD08:
        case PIN_PAD09:
        case PIN_PAD10:
        case PIN_PAD11:
        case PIN_PAD12:
        case PIN_PAD13:
        case PIN_PAD14:
        case PIN_PAD15:
            offset = pin_number - PIN_PAD08;
            assert(offset < 8);
            adc = 8+offset;
            if (pin_type == pin_type_analog_input) {
                ATD1DIEN &= ~(1<<offset);
            } else {
                assert(pin_type == pin_type_digital_input);
                ATD1DIEN |= 1<<offset;
            }
            break;
        case PIN_PA0:
        case PIN_PA1:
        case PIN_PA2:
        case PIN_PA3:
        case PIN_PA4:
        case PIN_PA5:
        case PIN_PA6:
        case PIN_PA7:
            offset = pin_number - PIN_PA0;
            assert(offset < 8);
            if (pin_type == pin_type_digital_output) {
                DDRA |= 1<<offset;
            } else {
                assert(pin_type == pin_type_digital_input);
                DDRA &= ~(1<<offset);
            }
            break;
        case PIN_PB0:
        case PIN_PB1:
        case PIN_PB2:
        case PIN_PB3:
        case PIN_PB4:
        case PIN_PB5:
        case PIN_PB6:
        case PIN_PB7:
            offset = pin_number - PIN_PB0;
            assert(offset < 8);
            if (pin_type == pin_type_digital_output) {
                DDRB |= 1<<offset;
            } else {
                assert(pin_type == pin_type_digital_input);
                DDRB &= ~(1<<offset);
            }
            break;
        case PIN_PE0:
        case PIN_PE1:
        case PIN_PE2:
        case PIN_PE3:
        case PIN_PE4:
        case PIN_PE5:
        case PIN_PE6:
        case PIN_PE7:
            offset = pin_number - PIN_PE0;
            assert(offset < 8);
            if (pin_type == pin_type_digital_output) {
                DDRE |= 1<<offset;
            } else {
                assert(pin_type == pin_type_digital_input);
                DDRE &= ~(1<<offset);
            }
            break;
        case PIN_PH0:
        case PIN_PH1:
        case PIN_PH2:
        case PIN_PH3:
        case PIN_PH4:
        case PIN_PH5:
        case PIN_PH6:
        case PIN_PH7:
            offset = pin_number - PIN_PH0;
            assert(offset < 8);
            if (pin_type == pin_type_digital_output) {
                DDRH |= 1<<offset;
            } else {
                assert(pin_type == pin_type_digital_input);
                DDRH &= ~(1<<offset);
            }
            break;
        case PIN_PJ0:
        case PIN_PJ1:
        case PIN_PJ2:
        case PIN_PJ3:
        case PIN_PJ4:
        case PIN_PJ5:
        case PIN_PJ6:
        case PIN_PJ7:
            offset = pin_number - PIN_PJ0;
            assert(offset < 8);
            if (pin_type == pin_type_digital_output) {
                DDRJ |= 1<<offset;
            } else {
                assert(pin_type == pin_type_digital_input);
                DDRJ &= ~(1<<offset);
            }
            break;
        case PIN_PK0:
        case PIN_PK1:
        case PIN_PK2:
        case PIN_PK3:
        case PIN_PK4:
        case PIN_PK5:
        case PIN_PK6:
        case PIN_PK7:
            offset = pin_number - PIN_PK0;
            assert(offset < 8);
            if (pin_type == pin_type_digital_output) {
                DDRK |= 1<<offset;
            } else {
                assert(pin_type == pin_type_digital_input);
                DDRK &= ~(1<<offset);
            }
            break;
        case PIN_PM0:
        case PIN_PM1:
        case PIN_PM2:
        case PIN_PM3:
        case PIN_PM4:
        case PIN_PM5:
        case PIN_PM6:
        case PIN_PM7:
            offset = pin_number - PIN_PM0;
            assert(offset < 8);
            if (pin_type == pin_type_digital_output) {
                DDRM |= 1<<offset;
            } else {
                assert(pin_type == pin_type_digital_input);
                DDRM &= ~(1<<offset);
            }
            break;
        case PIN_PP0:
        case PIN_PP1:
        case PIN_PP2:
        case PIN_PP3:
        case PIN_PP4:
        case PIN_PP5:
        case PIN_PP6:
        case PIN_PP7:
            offset = pin_number - PIN_PP0;
            assert(offset < 8);
            if (pin_type == pin_type_analog_output) {
                PWME |= 1<<offset;
                PWMCLK &= ~(1 << offset);  // prescaled clock
            } else if (pin_type == pin_type_servo_output) {
                PWME |= 1<<offset;
                PWMCLK |= 1 << offset;  // prescaled and scaled clock
            } else {
                PWME &= ~(1<<offset);
                if (pin_type == pin_type_digital_output) {
                    DDRP |= 1<<offset;
                } else {
                    assert(pin_type == pin_type_digital_input);
                    DDRP &= ~(1<<offset);
                }
            }
            break;
        case PIN_PS0:
        case PIN_PS1:
        case PIN_PS2:
        case PIN_PS3:
        case PIN_PS4:
        case PIN_PS5:
        case PIN_PS6:
        case PIN_PS7:
            offset = pin_number - PIN_PS0;
            assert(offset < 8);
            if (pin_type == pin_type_uart_input) {
                if (offset == 0) {
                    SCI1C2X |= SCI1C2_RE_MASK;
                } else {
                    assert(offset == 2);
                    SCI2C2X |= SCI1C2_RE_MASK;
                }
            } else if (pin_type == pin_type_uart_output) {
                if (offset == 1) {
                    SCI1C2X |= SCI1C2_TE_MASK;
                } else {
                    assert(offset == 3);
                    SCI2C2X |= SCI1C2_TE_MASK;
                }
            } else {
                if (offset == 0) {
                    SCI1C2X &= ~SCI1C2_RE_MASK;
                } else if (offset == 1) {
                    SCI1C2X &= ~SCI1C2_TE_MASK;
                } else if (offset == 2) {
                    SCI2C2X &= ~SCI1C2_RE_MASK;
                } else if (offset == 3) {
                    SCI2C2X &= ~SCI1C2_TE_MASK;
                }
                if (pin_type == pin_type_digital_output) {
                    DDRS |= 1<<offset;
                } else {
                    assert(pin_type == pin_type_digital_input);
                    DDRS &= ~(1<<offset);
                }
            }
            break;
        case PIN_PT0:
        case PIN_PT1:
        case PIN_PT2:
        case PIN_PT3:
        case PIN_PT4:
        case PIN_PT5:
        case PIN_PT6:
        case PIN_PT7:
            offset = pin_number - PIN_PT0;
            assert(offset < 8);
            if (pin_type == pin_type_frequency_output) {
                if (freq[0]) {
                    printf("conflicting timer usage\n");
#if STICKOS
                    stop();
#endif
                } else {
                    if (offset < 4) {
                        TCTL2 = (TCTL2 & ~(3<<(offset*2))) | (1<<(offset*2));
                    } else {
                        TCTL1 = (TCTL1 & ~(3<<((offset-4)*2))) | (1<<((offset-4)*2));
                    }
                    freq[0] = pin_type_frequency_output;
                    // disable timer
                    TSCR1 = 0x00;
                }
            } else {
                if (offset < 4) {
                    TCTL2 = (TCTL2 & ~(3<<(offset*2)));
                } else {
                    TCTL1 = (TCTL1 & ~(3<<((offset-4)*2)));
                }
                if (pin_type == pin_type_digital_output) {
                    DDRT |= 1<<offset;
                } else {
                    assert(pin_type == pin_type_digital_input);
                    DDRT &= ~(1<<offset);
                }
            }
            break;
        default:
            assert(0);
            break;
    }
#elif PIC32
    // configure the PIC32 pin for the requested function
    switch (pin_number) {
#ifdef PIC32PORTA
        case PIN_RA0:
        case PIN_RA1:
        case PIN_RA2:
        case PIN_RA3:
        case PIN_RA4:
        case PIN_RA5:
        case PIN_RA6:
        case PIN_RA7:
        case PIN_RA9:
        case PIN_RA10:
        case PIN_RA14:
        case PIN_RA15:
            offset = pin_number - PIN_RA0;
            assert(offset < 16);
            if (pin_type == pin_type_digital_output) {
                TRISACLR = 1<<offset;
            } else {
                assert(pin_type == pin_type_digital_input);
                TRISASET = 1<<offset;
            }
            break;
#endif
        case PIN_AN0:
        case PIN_AN1:
        case PIN_AN2:
        case PIN_AN3:
        case PIN_AN4:
        case PIN_AN5:
        case PIN_AN6:
        case PIN_AN7:
        case PIN_AN8:
        case PIN_AN9:
        case PIN_AN10:
        case PIN_AN11:
        case PIN_AN12:
        case PIN_AN13:
        case PIN_AN14:
        case PIN_AN15:
#if PIC32PPS
            break;
#else
            offset = pin_number - PIN_AN0;
            assert(offset < 16);
            // if we have a pullup (see datasheet table 12-11)...
            if (offset < 6 || offset == 15) {
                if (pin_type == pin_type_digital_input) {
                    // turn on the pullup
                    CNPUESET = 1<<(offset<6?2+offset:12);
                } else {
                    // turn off the pullup
                    CNPUECLR = 1<<(offset<6?2+offset:12);
                }
            }
            if (pin_type == pin_type_analog_input) {
                AD1PCFGCLR = 1<<offset;
                TRISBSET = 1<<offset;
            } else {
                AD1PCFGSET = 1<<offset;
                if (pin_type == pin_type_digital_output) {
                    TRISBCLR = 1<<offset;
                } else {
                    assert(pin_type == pin_type_digital_input);
                    TRISBSET = 1<<offset;
                }
            }
            break;
        case PIN_RC1:
        case PIN_RC2:
        case PIN_RC3:
        case PIN_RC4:
        case PIN_RC13:
        case PIN_RC14:
            offset = pin_number - PIN_RC0;
            assert(offset < 16);
            // if we have a pullup (see datasheet table 12-11)...
            if (offset == 13 || offset == 14) {
                if (pin_type == pin_type_digital_input) {
                    // turn on the pullup
                    CNPUESET = 1<<(offset==13?1:0);
                } else {
                    // turn off the pullup
                    CNPUECLR = 1<<(offset==13?1:0);
                }
            }
            if (pin_type == pin_type_digital_output) {
                TRISCCLR = 1<<offset;
            } else {
                assert(pin_type == pin_type_digital_input);
                TRISCSET = 1<<offset;
            }
            break;
        case PIN_RD0:
        case PIN_RD1:
        case PIN_RD2:
        case PIN_RD3:
        case PIN_RD4:
        case PIN_RD5:
        case PIN_RD6:
        case PIN_RD7:
        case PIN_RD8:
        case PIN_RD9:
        case PIN_RD10:
        case PIN_RD11:
        case PIN_RD12:
        case PIN_RD13:
        case PIN_RD14:
        case PIN_RD15:
            offset = pin_number - PIN_RD0;
            assert(offset < 16);
            // if we have a pullup (see datasheet table 12-11)...
            if ((offset >= 4 && offset < 8) || (offset >= 13)) {
                if (pin_type == pin_type_digital_input) {
                    // turn on the pullup
                    CNPUESET = 1<<(offset>=13?19+offset-13:13+offset-4);
                } else {
                    // turn off the pullup
                    CNPUECLR = (offset>=13?19+offset-13:13+offset-4);
                }
            }
            if (pin_type == pin_type_analog_output || pin_type == pin_type_servo_output) {
                if (freq[0] && freq[0] != pin_type) {
                    printf("conflicting timer usage\n");
#if STICKOS
                    stop();
#endif
                } else {
                    if (! freq[0]) {
                        if (pin_type == pin_type_analog_output) {
                            // configure timer 2 for analog pwm generation
                            // set pwm prescale to 1
                            T2CON = 0;
                            TMR2 = 0;
                            PR2 = pin_analog-1;
                            T2CON = _T2CON_ON_MASK;
                        } else {
                            // configure timer 2 for servo pwm generation
                            // set pwm prescale to 64
                            T2CON = 0;
                            TMR2 = 0;
                            PR2 = SERVO_MOD-1;
                            assert(SERVO_PRESCALE == 64);
                            T2CON = _T2CON_ON_MASK|(6 << _T2CON_TCKPS_POSITION);
                        }
                        freq[0] = pin_type;
                    }
                }
            } else if (pin_type == pin_type_frequency_output) {
                if (! freq[1]) {
                    freq[1] = pin_type_frequency_output;
                    // NULL
                } else {
                    printf("conflicting timer usage\n");
#if STICKOS
                    stop();
#endif
                }
            } else {
                if (offset == 0) {
                    OC1CONCLR = _OC1CON_ON_MASK;
                } else if (offset == 1) {
                    OC2CONCLR = _OC2CON_ON_MASK;
                } else if (offset == 2) {
                    OC3CONCLR = _OC3CON_ON_MASK;
                } else if (offset == 3) {
                   OC4CONCLR = _OC4CON_ON_MASK;
                } else if (offset == 4) {
                    OC5CONCLR = _OC5CON_ON_MASK;
                }

                if (pin_type == pin_type_digital_output) {
                    TRISDCLR = 1<<offset;
                } else {
                    assert(pin_type == pin_type_digital_input);
                    TRISDSET = 1<<offset;
                }
            }        
            break;
        case PIN_RE0:
        case PIN_RE1:
        case PIN_RE2:
        case PIN_RE3:
        case PIN_RE4:
        case PIN_RE5:
        case PIN_RE6:
        case PIN_RE7:
        case PIN_RE8:
        case PIN_RE9:
            offset = pin_number - PIN_RE0;
            assert(offset < 16);
            if (pin_type == pin_type_digital_output) {
                TRISECLR = 1<<offset;
            } else {
                assert(pin_type == pin_type_digital_input);
                TRISESET = 1<<offset;
            }
            break;
        case PIN_RF0:
        case PIN_RF1:
        case PIN_RF2:
        case PIN_RF3:
        case PIN_RF4:
        case PIN_RF5:
        case PIN_RF6:
        case PIN_RF8:
        case PIN_RF12:
        case PIN_RF13:
            offset = pin_number - PIN_RF0;
            assert(offset < 16);
            // if we have a pullup (see datasheet table 12-11)...
            if (offset >= 4 && offset < 6) {
                if (pin_type == pin_type_digital_input) {
                    // turn on the pullup
                    CNPUESET = 1<<(17+offset-4);
                } else {
                    // turn off the pullup
                    CNPUECLR = (17+offset-4);
                }
            }
            if (pin_type == pin_type_uart_input || pin_type == pin_type_uart_output) {
                assert(offset == 4 || offset == 5);
                U2MODE |= _U2MODE_UARTEN_MASK;
            } else {
                if (offset == 4 || offset == 5) {
                    U2MODE &= ~_U2MODE_UARTEN_MASK;
                }

                if (pin_type == pin_type_digital_output) {
                    TRISFCLR = 1<<offset;
                } else {
                    assert(pin_type == pin_type_digital_input);
                    TRISFSET = 1<<offset;
                }
            }
            break;
        case PIN_RG0:
        case PIN_RG1:
        case PIN_RG6:
        case PIN_RG7:
        case PIN_RG8:
        case PIN_RG9:
        case PIN_RG12:
        case PIN_RG13:
        case PIN_RG14:
        case PIN_RG15:
            offset = pin_number - PIN_RG0;
            assert(offset < 16);
            // if we have a pullup (see datasheet table 12-11)...
            if (offset >= 6 && offset < 10) {
                if (pin_type == pin_type_digital_input) {
                    // turn on the pullup
                    CNPUESET = 1<<(8+offset-6);
                } else {
                    // turn off the pullup
                    CNPUECLR = (8+offset-6);
                }
            }
            if (pin_type == pin_type_digital_output) {
                TRISGCLR = 1<<offset;
            } else {
                assert(pin_type == pin_type_digital_input);
                TRISGSET = 1<<offset;
            }
#endif
            break;
        default:
            assert(0);
            break;
    }
#else
#error
#endif
#endif
}

#if MCF52221 || MCF52233 || MCF52259 || MCF5211
static
void
pwmset(int n, int32 value)
{
#if ! STICK_GUEST
    MCF_PWM_PWMDTY(n) = (byte)(value>>8);
    MCF_PWM_PWMDTY(n+1) = (byte)value;
#endif
}

static
int32
pwmget(int n)
{
#if ! STICK_GUEST
    return (MCF_PWM_PWMDTY(n)<<8) | MCF_PWM_PWMDTY(n+1);
#else
    return 0;
#endif
}
#endif

// this function declares a ram, flash, or pin variable!
void
pin_declare(IN int pin_number, IN int pin_type, IN int pin_qual)
{
    pin_declare_internal(pin_number, pin_type, pin_qual, false, true);
}

static byte ulasttx[MAX_UARTS];
static byte umask[MAX_UARTS];

#if MC9S08QE128 || MC9S12DT256 || MC9S12DP512
#pragma CODE_SEG __NEAR_SEG NON_BANKED
#endif
// this function sets a pin variable!
void
pin_set(IN int pin_number, IN int pin_type, IN int pin_qual, IN int32 value)
{
#if ! STICK_GUEST && ! SHRINK
#if MCF51CN128 || MCF51QE128 || MC9S08QE128
    int n;
#endif
    int32 value2;
    uint32 offset;
    
    if (pin_number == PIN_UNASSIGNED) {
        return;
    }
    
    if (pin_type == pin_type_analog_output) {
        // trim the analog level
        if (value < 0) {
            value = 0;
        } else if (value > pin_analog) {
            value = pin_analog;
        }
    } else if (pin_type == pin_type_servo_output) {
        // trim the servo level
        if (value < 0) {
            value = 0;
        } else if (value > SERVO_MAX) {
            value = SERVO_MAX;
        }
    } else if (pin_type == pin_type_frequency_output) {
        // trim the frequency
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
        if (value < 0) {
            value = 0;
        }
        if (value) {
            value = bus_frequency/value;
            if (value) {
                value--;
            }
        }
        if (! value) {
            value = 0xffffffff;
        }
#else
        if (value < 0) {
            value = 0;
        }
        if (value) {
            value = bus_frequency/FREQ_PRESCALE/value/2;
            if (value) {
                value--;
            }
        }
        if (value > 0xffff) {
            value = 0xffff;
        }
#endif
    }
    
    if (pin_qual & 1<<pin_qual_inverted) {
        if (pin_type == pin_type_digital_output) {
            value2 = value;
            value = ! value;
            ASSERT(value != value2);  // catch CW bug
        } else if (pin_type == pin_type_analog_output) {
            value2 = value;
            value = pin_analog-value;
            ASSERT(value+value2 == pin_analog);  // catch CW bug
        } else if (pin_type == pin_type_servo_output) {
            value2 = value;
            value = SERVO_MAX-value;
            ASSERT(value+value2 == SERVO_MAX);  // catch CW bug
        }
    }

    // If setting to 1, then disable the driver before setting the data value to 1.  This avoids having the processor drive the
    // open_drain pin, which would be bad if the line is held high by another driver.
    if (value && (pin_qual & 1<<pin_qual_open_drain)) {
        assert(pin_type == pin_type_digital_output);
        pin_declare_internal(pin_number, pin_type_digital_input, pin_qual, true, false);
    }
    
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    // set the MCF52221/MCF52233/MCF52259/MCF5211 pin to value
    switch (pin_number) {
        case PIN_DTIN0:
        case PIN_DTIN1:
        case PIN_DTIN2:
        case PIN_DTIN3:
            offset = pin_number - PIN_DTIN0;
            assert(offset < 4);
            if (pin_type == pin_type_analog_output) {
                // program MCF_PWM_PWMDTY with values 0 (3.3v) to pin_analog (0v)
                pwmset(offset*2, pin_analog - value);
            } else if (pin_type == pin_type_servo_output) {
                // program MCF_PWM_PWMDTY with values 0 (SERVO_MAX) to SERVO_MOD (0)
                pwmset(offset*2, SERVO_MOD - value*SERVO_MOD/SERVO_MAX);
            } else if (pin_type == pin_type_frequency_output) {
                // program MCF_DTIM_DTRR with bus_frequency (1 Hz) to 1 (bus_frequency Hz)
                MCF_DTIM_DTRR(offset) = value;
                
                // catch missed wraps
                if (MCF_DTIM_DTCN(offset) >= MCF_DTIM_DTRR(offset)) {
                    MCF_DTIM_DTCN(offset) = 0;
                }
            } else {
                assert(pin_type == pin_type_digital_output);
                if (value) {
                    MCF_GPIO_SETTC = 1 << offset;
                } else {
                    MCF_GPIO_CLRTC = ~(1 << offset);
                }
            }
            break;
        case PIN_QSPI_DOUT:
        case PIN_QSPI_DIN:
        case PIN_QSPI_CLK:
        case PIN_QSPI_CS0:
#if MCF52259
        case PIN_QSPI_CS2:
        case PIN_QSPI_CS3:
#endif
            offset = pin_number - PIN_QSPI_DOUT;
            assert(offset < 8);
            assert(pin_type == pin_type_digital_output);
            if (value) {
                MCF_GPIO_SETQS = 1 << offset;
            } else {
                MCF_GPIO_CLRQS = ~(1 << offset);
            }
            break;
#if MCF52259
        case PIN_FEC_COL:
        case PIN_FEC_CRS:
        case PIN_FEC_RXCLK:
        case PIN_FEC_RXD0:
        case PIN_FEC_RXD1:
        case PIN_FEC_RXD2:
        case PIN_FEC_RXD3:
        case PIN_FEC_RXDV:
            offset = pin_number - PIN_FEC_COL;
            assert(offset < 8);
            assert(pin_type == pin_type_digital_output);
            if (value) {
                MCF_GPIO_SETTI = 1 << offset;
            } else {
                MCF_GPIO_CLRTI = ~(1 << offset);
            }
            break;
        case PIN_FEC_RXER:
        case PIN_FEC_TXCLK:
        case PIN_FEC_TXD0:
        case PIN_FEC_TXD1:
        case PIN_FEC_TXD2:
        case PIN_FEC_TXD3:
        case PIN_FEC_TXEN:
        case PIN_FEC_TXER:
            offset = pin_number - PIN_FEC_RXER;
            assert(offset < 8);
            assert(pin_type == pin_type_digital_output);
            if (value) {
                MCF_GPIO_SETTJ = 1 << offset;
            } else {
                MCF_GPIO_CLRTJ = ~(1 << offset);
            }
            break;
        case PIN_UTXD2:
        case PIN_URXD2:
        case PIN_RTS2:
        case PIN_CTS2:
            offset = pin_number - PIN_UTXD2;
            assert(offset < 4);
            if (pin_type == pin_type_uart_output) {
                assert(pin_number == PIN_UTXD2);
                pin_uart_tx(2, value);
            } else {
                assert(pin_type == pin_type_digital_output);
                if (value) {
                    MCF_GPIO_SETUC = 1 << offset;
                } else {
                    MCF_GPIO_CLRUC = ~(1 << offset);
                }
            }
            break;
#endif
        case PIN_UTXD1:
        case PIN_URXD1:
        case PIN_RTS1:
        case PIN_CTS1:
            offset = pin_number - PIN_UTXD1;
            assert(offset < 4);
            if (pin_type == pin_type_uart_output) {
                assert(pin_number == PIN_UTXD1);
                pin_uart_tx(1, value);
            } else {
                assert(pin_type == pin_type_digital_output);
                if (value) {
                    MCF_GPIO_SETUB = 1 << offset;
                } else {
                    MCF_GPIO_CLRUB = ~(1 << offset);
                }
            }
            break;
        case PIN_UTXD0:
        case PIN_URXD0:
        case PIN_RTS0:
        case PIN_CTS0:
            offset = pin_number - PIN_UTXD0;
            assert(offset < 4);
            if (pin_type == pin_type_uart_output) {
                assert(pin_number == PIN_UTXD0);
                pin_uart_tx(0, value);
            } else {
                assert(pin_type == pin_type_digital_output);
                if (value) {
                    MCF_GPIO_SETUA = 1 << offset;
                } else {
                    MCF_GPIO_CLRUA = ~(1 << offset);
                }
            }
            break;
        case PIN_AN0:
        case PIN_AN1:
        case PIN_AN2:
        case PIN_AN3:
        case PIN_AN4:
        case PIN_AN5:
        case PIN_AN6:
        case PIN_AN7:
            offset = pin_number - PIN_AN0;
            assert(offset < 8);
            assert(pin_type == pin_type_digital_output);
            if (value) {
                MCF_GPIO_SETAN = 1 << offset;
            } else {
                MCF_GPIO_CLRAN = ~(1 << offset);
            }
            break;
        case PIN_IRQ1:
        case PIN_IRQ2:
        case PIN_IRQ3:
        case PIN_IRQ4:
        case PIN_IRQ5:
        case PIN_IRQ6:
        case PIN_IRQ7:
            offset = pin_number - PIN_IRQ0;
            assert(offset < 8);
            assert(pin_type == pin_type_digital_output);
            if (value) {
                MCF_GPIO_SETNQ = 1 << offset;
            } else {
                MCF_GPIO_CLRNQ = ~(1 << offset);
            }
            break;
#if MCF52233
        case PIN_IRQ11:
            offset = pin_number - PIN_IRQ8;
            assert(offset < 8);
            assert(pin_type == pin_type_digital_output);
            if (value) {
                MCF_GPIO_SETGP = 1 << offset;
            } else {
                MCF_GPIO_CLRGP = ~(1 << offset);
            }
            break;
#endif
#if MCF52233 || MCF52259 || MCF5211
        case PIN_GPT0:
        case PIN_GPT1:
        case PIN_GPT2:
        case PIN_GPT3:
            offset = pin_number - PIN_GPT0;
            assert(offset < 4);
            assert(pin_type == pin_type_digital_output);
            if (value) {
                MCF_GPIO_SETTA = 1 << offset;
            } else {
                MCF_GPIO_CLRTA = ~(1 << offset);
            }
            break;
#endif
        case PIN_SCL:
        case PIN_SDA:
            offset = pin_number - PIN_SCL;
            assert(offset < 2);
            assert(pin_type == pin_type_digital_output);
            if (value) {
                MCF_GPIO_SETAS = 1 << offset;
            } else {
                MCF_GPIO_CLRAS = ~(1 << offset);
            }
            break;
        default:
            assert(0);
            break;
    }
#elif MCF51JM128
    // set the MCF51JM128 pin to value
    switch (pin_number) {
        case PIN_PTA0:
        case PIN_PTA1:
        case PIN_PTA2:
        case PIN_PTA3:
        case PIN_PTA4:
        case PIN_PTA5:
            offset = pin_number - PIN_PTA0;
            assert(offset < 6);
            assert(pin_type == pin_type_digital_output);
            if (value) {
                PTAD |= 1<<offset;
            } else {
                PTAD &= ~(1<<offset);
            }
            break;
        case PIN_PTB0:
        case PIN_PTB1:
        case PIN_PTB2:
        case PIN_PTB3:
        case PIN_PTB4:
        case PIN_PTB5:
        case PIN_PTB6:
        case PIN_PTB7:
            offset = pin_number - PIN_PTB0;
            assert(offset < 8);
            assert(pin_type == pin_type_digital_output);
            if (value) {
                PTBD |= 1<<offset;
            } else {
                PTBD &= ~(1<<offset);
            }
            break;
        case PIN_PTC0:
        case PIN_PTC1:
        case PIN_PTC2:
        case PIN_PTC3:  // u2tx
        case PIN_PTC4:
        case PIN_PTC5:
        case PIN_PTC6:
            offset = pin_number - PIN_PTC0;
            assert(offset < 8);
            if (pin_type == pin_type_uart_output) {
                assert(offset == 3);
                pin_uart_tx(1, value);
            } else {
                assert(pin_type == pin_type_digital_output);
                if (value) {
                    PTCD |= 1<<offset;
                } else {
                    PTCD &= ~(1<<offset);
                }
            }
            break;
        case PIN_PTD0:
        case PIN_PTD1:
        case PIN_PTD2:
        case PIN_PTD3:
        case PIN_PTD4:
        case PIN_PTD5:
        case PIN_PTD6:
        case PIN_PTD7:
            offset = pin_number - PIN_PTD0;
            assert(offset < 8);
            assert(pin_type == pin_type_digital_output);
            if (value) {
                PTDD |= 1<<offset;
            } else {
                PTDD &= ~(1<<offset);
            }
            break;
        case PIN_PTE0:  // u1tx
        case PIN_PTE1:
        case PIN_PTE2:
        case PIN_PTE3:
        case PIN_PTE4:
        case PIN_PTE5:
        case PIN_PTE6:
        case PIN_PTE7:
            offset = pin_number - PIN_PTE0;
            assert(offset < 8);
            if (pin_type == pin_type_analog_output || pin_type == pin_type_servo_output || pin_type == pin_type_frequency_output) {
                assert(offset >= 2 && offset < 4);
                if (pin_type == pin_type_frequency_output) {
                    // program the counter for frequency generation (private counter)
                    TPM1MODH = value>>8;
                    TPM1MODL = value&0xff;
                    TPM1CNTL = 0;

                    // hack to work around high minimum frequency
                    if (! value) {
                        // temporarily revert to gpio
                        if (offset == 2) {
                            TPM1C0SC_ELS0x = 0;
                        } else {
                            TPM1C1SC_ELS1x = 0;
                        }
                    } else {
                        // the pin really is frequency
                        if (offset == 2) {
                            TPM1C0SC_ELS0x = 1;
                        } else {
                            TPM1C1SC_ELS1x = 1;
                        }
                    }
                } else if (pin_type == pin_type_servo_output) {
                    value = value*SERVO_MOD/SERVO_MAX;
                }
                
                // set the channel frequency or analog level
                if (offset == 2) {
                    TPM1C0VH = value>>8;
                    TPM1C0VL = value&0xff;
                } else {
                    TPM1C1VH = value>>8;
                    TPM1C1VL = value&0xff;
                }
            } else if (pin_type == pin_type_uart_output) {
                assert(offset == 0);
                pin_uart_tx(0, value);
            } else {
                assert(pin_type == pin_type_digital_output);
                if (value) {
                    PTED |= 1<<offset;
                } else {
                    PTED &= ~(1<<offset);
                }
            }
            break;
        case PIN_PTF0:
        case PIN_PTF1:
        case PIN_PTF2:
        case PIN_PTF3:
        case PIN_PTF4:
        case PIN_PTF5:
        case PIN_PTF6:
        case PIN_PTF7:
            offset = pin_number - PIN_PTF0;
            assert(offset < 8);
            if (pin_type == pin_type_analog_output || pin_type == pin_type_servo_output || pin_type == pin_type_frequency_output) {
                assert(offset < 6);
                if (pin_type == pin_type_frequency_output) {
                    // program the counter for frequency generation (private counter)
                    if (offset >= 4) {
                        TPM2MODH = value>>8;
                        TPM2MODL = value&0xff;
                        TPM2CNTL = 0;
                    } else {
                        TPM1MODH = value>>8;
                        TPM1MODL = value&0xff;
                        TPM1CNTL = 0;
                    }
                    
                    // hack to work around high minimum frequency
                    if (! value) {
                        // temporarily revert to gpio
                        if (offset == 0) {
                            TPM1C2SC_ELS2x = 0;
                        } else if (offset == 1) {
                            TPM1C3SC_ELS3x = 0;
                        } else if (offset == 2) {
                            TPM1C4SC_ELS4x = 0;
                        } else if (offset == 3) {
                            TPM1C5SC_ELS5x = 0;
                        } else if (offset == 4) {
                            TPM2C0SC_ELS0x = 0;
                        } else {
                            TPM2C1SC_ELS1x = 0;
                        }
                    } else {
                        // the pin really is frequency
                        if (offset == 0) {
                            TPM1C2SC_ELS2x = 1;
                        } else if (offset == 1) {
                            TPM1C3SC_ELS3x = 1;
                        } else if (offset == 2) {
                            TPM1C4SC_ELS4x = 1;
                        } else if (offset == 3) {
                            TPM1C5SC_ELS5x = 1;
                        } else if (offset == 4) {
                            TPM2C0SC_ELS0x = 1;
                        } else {
                            TPM2C1SC_ELS1x = 1;
                        }
                    }
                } else if (pin_type == pin_type_servo_output) {
                    value = value*SERVO_MOD/SERVO_MAX;
                }
                
                // set the channel frequency or analog level
                if (offset == 0) {
                    TPM1C2VH = value>>8;
                    TPM1C2VL = value&0xff;
                } else if (offset == 1) {
                    TPM1C3VH = value>>8;
                    TPM1C3VL = value&0xff;
                } else if (offset == 2) {
                    TPM1C4VH = value>>8;
                    TPM1C4VL = value&0xff;
                } else if (offset == 3) {
                    TPM1C5VH = value>>8;
                    TPM1C5VL = value&0xff;
                } else if (offset == 4) {
                    TPM2C0VH = value>>8;
                    TPM2C0VL = value&0xff;
                } else {
                    TPM2C1VH = value>>8;
                    TPM2C1VL = value&0xff;
                }
            } else {
                assert(pin_type == pin_type_digital_output);
                if (value) {
                    PTFD |= 1<<offset;
                } else {
                    PTFD &= ~(1<<offset);
                }
            }
            break;
        case PIN_PTG0:
        case PIN_PTG1:
        case PIN_PTG2:
        case PIN_PTG3:
            offset = pin_number - PIN_PTG0;
            assert(offset < 4);
            assert(pin_type == pin_type_digital_output);
            if (value) {
                PTGD |= 1<<offset;
            } else {
                PTGD &= ~(1<<offset);
            }
            break;
        default:
            assert(0);
            break;
    }
#elif MCF51CN128
    // set the MCF51CN128 pin to value
    switch (pin_number) {
        case PIN_PTA0:
        case PIN_PTA1:
        case PIN_PTA2:
        case PIN_PTA3:  // u3tx
        case PIN_PTA4:  // u3rx
        case PIN_PTA5:
        case PIN_PTA6:
        case PIN_PTA7:
            offset = pin_number - PIN_PTA0;
            assert(offset < 8);
            if (pin_type == pin_type_uart_output) {
                assert(offset == 3);
                pin_uart_tx(2, value);
            } else {
                assert(pin_type == pin_type_digital_output);
                if (value) {
                    PTAD |= 1<<offset;
                } else {
                    PTAD &= ~(1<<offset);
                }
            }
            break;
        case PIN_PTB0:
        case PIN_PTB1:
        case PIN_PTB2:
        case PIN_PTB3:
        case PIN_PTB4:
        case PIN_PTB5:
        case PIN_PTB6:  // tpm2ch0
        case PIN_PTB7:  // tpm2ch1
            offset = pin_number - PIN_PTB0;
            assert(offset < 8);
            if (pin_type == pin_type_analog_output || pin_type == pin_type_servo_output || pin_type == pin_type_frequency_output) {
                assert(offset >= 6 || offset < 8);
                n = 1;
                if (pin_type == pin_type_frequency_output) {
                    // program the counter for frequency generation (private counter)
                    assert(n);
                    TPM2MODH = value>>8;
                    TPM2MODL = value&0xff;
                    TPM2CNTL = 0;
                    
                    // hack to work around high minimum frequency
                    if (! value) {
                        // temporarily revert to gpio
                        if (offset == 6) {
                            TPM2C0SC_ELS0x = 0;
                        } else {
                            assert(offset == 7);
                            TPM2C1SC_ELS1x = 0;
                        }
                    } else {
                        // the pin really is frequency
                        if (offset == 6) {
                            TPM2C0SC_ELS0x = 1;
                        } else {
                            assert(offset == 7);
                            TPM2C1SC_ELS1x = 1;
                        }
                    }
                } else if (pin_type == pin_type_servo_output) {
                    value = value*SERVO_MOD/SERVO_MAX;
                }
                
                // set the channel frequency or analog level
                if (offset == 6) {
                    TPM2C0VH = value>>8;
                    TPM2C0VL = value&0xff;
                } else {
                    assert(offset == 7);
                    TPM2C1VH = value>>8;
                    TPM2C1VL = value&0xff;
                }
            } else {
                assert(pin_type == pin_type_digital_output);
                if (value) {
                    PTBD |= 1<<offset;
                } else {
                    PTBD &= ~(1<<offset);
                }
            }
            break;
        case PIN_PTC0:  // tpm2ch2
        case PIN_PTC1:
        case PIN_PTC2:
        case PIN_PTC3:
        case PIN_PTC4:  // adc11
        case PIN_PTC5:  // adc10
        case PIN_PTC6:  // adc9
        case PIN_PTC7:  // adc8
            offset = pin_number - PIN_PTC0;
            assert(offset < 8);
            if (pin_type == pin_type_analog_output || pin_type == pin_type_servo_output || pin_type == pin_type_frequency_output) {
                assert(offset == 0);
                n = 1;
                if (pin_type == pin_type_frequency_output) {
                    // program the counter for frequency generation (private counter)
                    assert(n);
                    TPM2MODH = value>>8;
                    TPM2MODL = value&0xff;
                    TPM2CNTL = 0;
                    
                    // hack to work around high minimum frequency
                    if (! value) {
                        // temporarily revert to gpio
                        assert(! offset);
                        TPM2C2SC_ELS2x = 0;
                    } else {
                        // the pin really is frequency
                        assert(offset == 0);
                        TPM2C2SC_ELS2x = 1;
                    }
                } else if (pin_type == pin_type_servo_output) {
                    value = value*SERVO_MOD/SERVO_MAX;
                }
                
                // set the channel frequency or analog level
                assert(offset == 0);
                TPM2C2VH = value>>8;
                TPM2C2VL = value&0xff;
            } else {
                assert(pin_type == pin_type_digital_output);
                if (value) {
                    PTCD |= 1<<offset;
                } else {
                    PTCD &= ~(1<<offset);
                }
            }
            break;
        case PIN_PTD0:  // adc7, u1tx
        case PIN_PTD1:  // adc6, u1rx
        case PIN_PTD2:  // adc5, u2tx
        case PIN_PTD3:  // adc4, u2rx
        case PIN_PTD4:
        case PIN_PTD5:
        case PIN_PTD6:
        case PIN_PTD7:  // adc3
            offset = pin_number - PIN_PTD0;
            assert(offset < 8);
            if (pin_type == pin_type_uart_output) {
                assert(offset == 0 || offset == 2);
                if (offset == 0) {
                    pin_uart_tx(0, value);
                } else {
                    pin_uart_tx(1, value);
                }
            } else {
                assert(pin_type == pin_type_digital_output);
                if (value) {
                    PTDD |= 1<<offset;
                } else {
                    PTDD &= ~(1<<offset);
                }
            }
            break;
        case PIN_PTE0:  // adc2
        case PIN_PTE1:  // adc1
        case PIN_PTE2:  // adc0
        case PIN_PTE3:  // tpm1ch0
        case PIN_PTE4:  // tpm1ch1
        case PIN_PTE5:  // tpm1ch2
        case PIN_PTE6:
        case PIN_PTE7:
            offset = pin_number - PIN_PTE0;
            assert(offset < 8);
            if (pin_type == pin_type_analog_output || pin_type == pin_type_servo_output || pin_type == pin_type_frequency_output) {
                assert(offset >= 3 || offset < 6);
                n = 0;
                if (pin_type == pin_type_frequency_output) {
                    // program the counter for frequency generation (private counter)
                    assert(! n);
                    TPM1MODH = value>>8;
                    TPM1MODL = value&0xff;
                    TPM1CNTL = 0;
                    
                    // hack to work around high minimum frequency
                    if (! value) {
                        // temporarily revert to gpio
                        if (offset == 3) {
                            TPM1C0SC_ELS0x = 0;
                        } else if (offset == 4) {
                            TPM1C1SC_ELS1x = 0;
                        } else {
                            assert(offset == 5);
                            TPM2C2SC_ELS2x = 0;
                        }
                    } else {
                        // the pin really is frequency
                        if (offset == 3) {
                            TPM1C0SC_ELS0x = 1;
                        } else if (offset == 4) {
                            TPM1C1SC_ELS1x = 1;
                        } else {
                            assert(offset == 5);
                            TPM1C2SC_ELS2x = 1;
                        }
                    }
                } else if (pin_type == pin_type_servo_output) {
                    value = value*SERVO_MOD/SERVO_MAX;
                }
                
                // set the channel frequency or analog level
                if (offset == 3) {
                    TPM1C0VH = value>>8;
                    TPM1C0VL = value&0xff;
                } else if (offset == 4) {
                    TPM1C1VH = value>>8;
                    TPM1C1VL = value&0xff;
                } else {
                    assert(offset == 5);
                    TPM1C2VH = value>>8;
                    TPM1C2VL = value&0xff;
                }
            } else {
                assert(pin_type == pin_type_digital_output);
                if (value) {
                    PTED |= 1<<offset;
                } else {
                    PTED &= ~(1<<offset);
                }
            }
            break;
        case PIN_PTF0:
        case PIN_PTF1:
        case PIN_PTF2:
        case PIN_PTF3:
        case PIN_PTF4:
        case PIN_PTF5:
        case PIN_PTF6:
        case PIN_PTF7:
            offset = pin_number - PIN_PTF0;
            assert(offset < 8);
            assert(pin_type == pin_type_digital_output);
            if (value) {
                PTFD |= 1<<offset;
            } else {
                PTFD &= ~(1<<offset);
            }
            break;
        case PIN_PTG0:
        case PIN_PTG1:
        case PIN_PTG2:
        case PIN_PTG3:
        case PIN_PTG4:
        case PIN_PTG5:
        case PIN_PTG6:
        case PIN_PTG7:
            offset = pin_number - PIN_PTG0;
            assert(offset < 8);
            assert(pin_type == pin_type_digital_output);
            if (value) {
                PTGD |= 1<<offset;
            } else {
                PTGD &= ~(1<<offset);
            }
            break;
        case PIN_PTH0:
        case PIN_PTH1:
        case PIN_PTH2:
        case PIN_PTH3:
        case PIN_PTH4:
        case PIN_PTH5:
        case PIN_PTH6:
        case PIN_PTH7:
            offset = pin_number - PIN_PTH0;
            assert(offset < 8);
            assert(pin_type == pin_type_digital_output);
            if (value) {
                PTHD |= 1<<offset;
            } else {
                PTHD &= ~(1<<offset);
            }
            break;
        case PIN_PTJ0:
        case PIN_PTJ1:
        case PIN_PTJ2:
        case PIN_PTJ3:
        case PIN_PTJ4:
        case PIN_PTJ5:
            offset = pin_number - PIN_PTJ0;
            assert(offset < 6);
            assert(pin_type == pin_type_digital_output);
            if (value) {
                PTJD |= 1<<offset;
            } else {
                PTJD &= ~(1<<offset);
            }
            break;
        default:
            assert(0);
            break;
    }
#elif MCF51QE128 || MC9S08QE128
    // set the MCF51QE128/MC9S08QE128 pin to value
    switch (pin_number) {
        case PIN_PTA0:  // tpm1ch0
        case PIN_PTA1:  // tpm2ch0
        case PIN_PTA2:
        case PIN_PTA3:
        case PIN_PTA4:
        case PIN_PTA5:
        case PIN_PTA6:  // tpm1ch2
        case PIN_PTA7:  // tmp2ch2
            offset = pin_number - PIN_PTA0;
            assert(offset < 8);
            if (pin_type == pin_type_analog_output || pin_type == pin_type_servo_output || pin_type == pin_type_frequency_output) {
                assert(offset < 2 || offset >= 6);
                n = offset&1;
                assert(n == 0 || n == 1);
                if (pin_type == pin_type_frequency_output) {
                    // program the counter for frequency generation (private counter)
                    if (n) {
                        TPM2MODH = value>>8;
                        TPM2MODL = value&0xff;
                        TPM2CNTL = 0;
                    } else {
                        TPM1MODH = value>>8;
                        TPM1MODL = value&0xff;
                        TPM1CNTL = 0;
                    }
                    
                    // hack to work around high minimum frequency
                    if (! value) {
                        // temporarily revert to gpio
                        if (offset == 0) {
                            TPM1C0SC_ELS0x = 0;
                        } else if (offset == 1) {
                            TPM2C0SC_ELS0x = 0;
                        } else if (offset == 6) {
                            TPM1C2SC_ELS2x = 0;
                        } else {
                            assert(offset == 7);
                            TPM2C2SC_ELS2x = 0;
                        }
                    } else {
                        // the pin really is frequency
                        if (offset == 0) {
                            TPM1C0SC_ELS0x = 1;
                        } else if (offset == 1) {
                            TPM2C0SC_ELS0x = 1;
                        } else if (offset == 6) {
                            TPM1C2SC_ELS2x = 1;
                        } else {
                            assert(offset == 7);
                            TPM2C2SC_ELS2x = 1;
                        }
                    }
                } else if (pin_type == pin_type_servo_output) {
                    value = value*SERVO_MOD/SERVO_MAX;
                }
                
                // set the channel frequency or analog level
                if (offset == 0) {
                    TPM1C0VH = value>>8;
                    TPM1C0VL = value&0xff;
                } else if (offset == 1) {
                    TPM2C0VH = value>>8;
                    TPM2C0VL = value&0xff;
                } else if (offset == 6) {
                    TPM1C2VH = value>>8;
                    TPM1C2VL = value&0xff;
                } else {
                    assert(offset == 7);
                    TPM2C2VH = value>>8;
                    TPM2C2VL = value&0xff;
                }
            } else {
                assert(pin_type == pin_type_digital_output);
                if (value) {
                    PTAD |= 1<<offset;
                } else {
                    PTAD &= ~(1<<offset);
                }
            }
            break;
        case PIN_PTB0:
        case PIN_PTB1:  // u1tx
        case PIN_PTB2:
        case PIN_PTB3:
        case PIN_PTB4:  // tpm2ch1
        case PIN_PTB5:  // tpm1ch1
        case PIN_PTB6:
        case PIN_PTB7:
            offset = pin_number - PIN_PTB0;
            assert(offset < 8);
            if (pin_type == pin_type_uart_output) {
                assert(offset == 1);
                pin_uart_tx(0, value);
            } else if (pin_type == pin_type_analog_output || pin_type == pin_type_servo_output || pin_type == pin_type_frequency_output) {
                assert(offset >= 4 || offset < 6);
                n = offset == 4;
                assert(n == 0 || n == 1);
                if (pin_type == pin_type_frequency_output) {
                    // program the counter for frequency generation (private counter)
                    if (n) {
                        TPM2MODH = value>>8;
                        TPM2MODL = value&0xff;
                        TPM2CNTL = 0;
                    } else {
                        TPM1MODH = value>>8;
                        TPM1MODL = value&0xff;
                        TPM1CNTL = 0;
                    }
                    
                    // hack to work around high minimum frequency
                    if (! value) {
                        // temporarily revert to gpio
                        if (offset == 5) {
                            TPM1C1SC_ELS1x = 0;
                        } else {
                            assert(offset == 4);
                            TPM2C1SC_ELS1x = 0;
                        }
                    } else {
                        // the pin really is frequency
                        if (offset == 5) {
                            TPM1C1SC_ELS1x = 1;
                        } else {
                            assert(offset == 4);
                            TPM2C1SC_ELS1x = 1;
                        }
                    }
                } else if (pin_type == pin_type_servo_output) {
                    value = value*SERVO_MOD/SERVO_MAX;
                }
                
                // set the channel frequency or analog level
                if (offset == 5) {
                    TPM1C1VH = value>>8;
                    TPM1C1VL = value&0xff;
                } else {
                    assert(offset == 4);
                    TPM2C1VH = value>>8;
                    TPM2C1VL = value&0xff;
                }
            } else {
                assert(pin_type == pin_type_digital_output);
                if (value) {
                    PTBD |= 1<<offset;
                } else {
                    PTBD &= ~(1<<offset);
                }
            }
            break;
        case PIN_PTC0:  // tpm3ch0
        case PIN_PTC1:  // tpm3ch1
        case PIN_PTC2:  // tpm3ch2
        case PIN_PTC3:  // tpm3ch3
        case PIN_PTC4:  // tpm3ch4
        case PIN_PTC5:  // tpm3ch5
        case PIN_PTC6:
        case PIN_PTC7:  // u2tx
            offset = pin_number - PIN_PTC0;
            assert(offset < 8);
            if (pin_type == pin_type_uart_output) {
                assert(offset == 7);
                pin_uart_tx(1, value);
            } else if (pin_type == pin_type_analog_output || pin_type == pin_type_servo_output || pin_type == pin_type_frequency_output) {
                assert(offset < 6);
                n = 2;
                if (pin_type == pin_type_frequency_output) {
                    // program the counter for frequency generation (private counter)
                    TPM3MODH = value>>8;
                    TPM3MODL = value&0xff;
                    TPM3CNTL = 0;
                    
                    // hack to work around high minimum frequency
                    if (! value) {
                        // temporarily revert to gpio
                        if (offset == 0) {
                            TPM3C0SC_ELS0x = 0;
                        } else if (offset == 1) {
                            TPM3C1SC_ELS1x = 0;
                        } else if (offset == 2) {
                            TPM3C2SC_ELS2x = 0;
                        } else if (offset == 3) {
                            TPM3C3SC_ELS3x = 0;
                        } else if (offset == 4) {
                            TPM3C4SC_ELS4x = 0;                            
                        } else {
                            assert(offset == 5);
                            TPM3C5SC_ELS5x = 0;
                        }
                    } else {
                        // the pin really is frequency
                        if (offset == 0) {
                            TPM3C0SC_ELS0x = 1;
                        } else if (offset == 1) {
                            TPM3C1SC_ELS1x = 1;
                        } else if (offset == 2) {
                            TPM3C2SC_ELS2x = 1;
                        } else if (offset == 3) {
                            TPM3C3SC_ELS3x = 1;
                        } else if (offset == 4) {
                            TPM3C4SC_ELS4x = 1;
                        } else {
                            assert(offset == 5);
                            TPM3C5SC_ELS5x = 1;
                        }
                    }
                } else if (pin_type == pin_type_servo_output) {
                    value = value*SERVO_MOD/SERVO_MAX;
                }
                
                // set the channel frequency or analog level
                if (offset == 0) {
                    TPM3C0VH = value>>8;
                    TPM3C0VL = value&0xff;
                } else if (offset == 1) {
                    TPM3C1VH = value>>8;
                    TPM3C1VL = value&0xff;
                } else if (offset == 2) {
                    TPM3C2VH = value>>8;
                    TPM3C2VL = value&0xff;
                } else if (offset == 3) {
                    TPM3C3VH = value>>8;
                    TPM3C3VL = value&0xff;
                } else if (offset == 4) {
                    TPM3C4VH = value>>8;
                    TPM3C4VL = value&0xff;
                } else {
                    assert(offset == 5);
                    TPM3C5VH = value>>8;
                    TPM3C5VL = value&0xff;
                }
            } else {
                assert(pin_type == pin_type_digital_output);
                if (value) {
                    PTCD |= 1<<offset;
                } else {
                    PTCD &= ~(1<<offset);
                }
            }
            break;
        case PIN_PTD0:
        case PIN_PTD1:
        case PIN_PTD2:
        case PIN_PTD3:
        case PIN_PTD4:
        case PIN_PTD5:
        case PIN_PTD6:
        case PIN_PTD7:
            offset = pin_number - PIN_PTD0;
            assert(offset < 8);
            assert(pin_type == pin_type_digital_output);
            if (value) {
                PTDD |= 1<<offset;
            } else {
                PTDD &= ~(1<<offset);
            }
            break;
        case PIN_PTE0:
        case PIN_PTE1:
        case PIN_PTE2:
        case PIN_PTE3:
        case PIN_PTE4:
        case PIN_PTE5:
        case PIN_PTE6:
        case PIN_PTE7:
            offset = pin_number - PIN_PTE0;
            assert(offset < 8);
            assert(pin_type == pin_type_digital_output);
            if (value) {
                PTED |= 1<<offset;
            } else {
                PTED &= ~(1<<offset);
            }
            break;
        case PIN_PTF0:
        case PIN_PTF1:
        case PIN_PTF2:
        case PIN_PTF3:
        case PIN_PTF4:
        case PIN_PTF5:
        case PIN_PTF6:
        case PIN_PTF7:
            offset = pin_number - PIN_PTF0;
            assert(offset < 8);
            assert(pin_type == pin_type_digital_output);
            if (value) {
                PTFD |= 1<<offset;
            } else {
                PTFD &= ~(1<<offset);
            }
            break;
        case PIN_PTG0:
        case PIN_PTG1:
        case PIN_PTG2:
        case PIN_PTG3:
            offset = pin_number - PIN_PTG0;
            assert(offset < 4);
            assert(pin_type == pin_type_digital_output);
            if (value) {
                PTGD |= 1<<offset;
            } else {
                PTGD &= ~(1<<offset);
            }
            break;
        default:
            assert(0);
            break;
    }
#elif MC9S12DT256 || MC9S12DP512
    // set the MC9S12DT256/MC9S12DP512 pin to value
    switch (pin_number) {
        case PIN_PAD00:
        case PIN_PAD01:
        case PIN_PAD02:
        case PIN_PAD03:
        case PIN_PAD04:
        case PIN_PAD05:
        case PIN_PAD06:
        case PIN_PAD07:
            assert(0);
            break;
        case PIN_PAD08:
        case PIN_PAD09:
        case PIN_PAD10:
        case PIN_PAD11:
        case PIN_PAD12:
        case PIN_PAD13:
        case PIN_PAD14:
        case PIN_PAD15:
            assert(0);
            break;
        case PIN_PA0:
        case PIN_PA1:
        case PIN_PA2:
        case PIN_PA3:
        case PIN_PA4:
        case PIN_PA5:
        case PIN_PA6:
        case PIN_PA7:
            offset = pin_number - PIN_PA0;
            assert(offset < 8);
            assert(pin_type == pin_type_digital_output);
            if (value) {
                PORTA |= 1<<offset;
            } else {
                PORTA &= ~(1<<offset);
            }
            break;
        case PIN_PB0:
        case PIN_PB1:
        case PIN_PB2:
        case PIN_PB3:
        case PIN_PB4:
        case PIN_PB5:
        case PIN_PB6:
        case PIN_PB7:
            offset = pin_number - PIN_PB0;
            assert(offset < 8);
            assert(pin_type == pin_type_digital_output);
            if (value) {
                PORTB |= 1<<offset;
            } else {
                PORTB &= ~(1<<offset);
            }
            break;
        case PIN_PE0:
        case PIN_PE1:
        case PIN_PE2:
        case PIN_PE3:
        case PIN_PE4:
        case PIN_PE5:
        case PIN_PE6:
        case PIN_PE7:
            offset = pin_number - PIN_PE0;
            assert(offset < 8);
            assert(pin_type == pin_type_digital_output);
            if (value) {
                PORTE |= 1<<offset;
            } else {
                PORTE &= ~(1<<offset);
            }
            break;
        case PIN_PH0:
        case PIN_PH1:
        case PIN_PH2:
        case PIN_PH3:
        case PIN_PH4:
        case PIN_PH5:
        case PIN_PH6:
        case PIN_PH7:
            offset = pin_number - PIN_PH0;
            assert(offset < 8);
            assert(pin_type == pin_type_digital_output);
            if (value) {
                PTH |= 1<<offset;
            } else {
                PTH &= ~(1<<offset);
            }
            break;
        case PIN_PJ0:
        case PIN_PJ1:
        case PIN_PJ2:
        case PIN_PJ3:
        case PIN_PJ4:
        case PIN_PJ5:
        case PIN_PJ6:
        case PIN_PJ7:
            offset = pin_number - PIN_PJ0;
            assert(offset < 8);
            assert(pin_type == pin_type_digital_output);
            if (value) {
                PTJ |= 1<<offset;
            } else {
                PTJ &= ~(1<<offset);
            }
            break;
        case PIN_PK0:
        case PIN_PK1:
        case PIN_PK2:
        case PIN_PK3:
        case PIN_PK4:
        case PIN_PK5:
        case PIN_PK6:
        case PIN_PK7:
            offset = pin_number - PIN_PK0;
            assert(offset < 8);
            assert(pin_type == pin_type_digital_output);
            if (value) {
                PORTK |= 1<<offset;
            } else {
                PORTK &= ~(1<<offset);
            }
            break;
        case PIN_PM0:
        case PIN_PM1:
        case PIN_PM2:
        case PIN_PM3:
        case PIN_PM4:
        case PIN_PM5:
        case PIN_PM6:
        case PIN_PM7:
            offset = pin_number - PIN_PM0;
            assert(offset < 8);
            assert(pin_type == pin_type_digital_output);
            if (value) {
                PTM |= 1<<offset;
            } else {
                PTM &= ~(1<<offset);
            }
            break;
        case PIN_PP0:
        case PIN_PP1:
        case PIN_PP2:
        case PIN_PP3:
        case PIN_PP4:
        case PIN_PP5:
        case PIN_PP6:
        case PIN_PP7:
            offset = pin_number - PIN_PP0;
            assert(offset < 8);
            if (pin_type == pin_type_analog_output || pin_type == pin_type_servo_output) {
                if (pin_type == pin_type_analog_output) {
                    // program PWMDTY with values 0 (3.3v) to 255 (0v)
                    value = 255 - value*255/pin_analog;
                } else {
                    assert(pin_type == pin_type_servo_output);
                    // program MCF_PWM_PWMDTY with values 0 (SERVO_MAX) to 255 (0)
                    value = 255 - value*255/SERVO_MAX;
                }
                switch (offset) {
                    case 0:
                        PWMDTY0 = value;
                        break;
                    case 1:
                        PWMDTY1 = value;
                        break;
                    case 2:
                        PWMDTY2 = value;
                        break;
                    case 3:
                        PWMDTY3 = value;
                        break;
                    case 4:
                        PWMDTY4 = value;
                        break;
                    case 5:
                        PWMDTY5 = value;
                        break;
                    case 6:
                        PWMDTY6 = value;
                        break;
                    case 7:
                        PWMDTY7 = value;
                        break;
                    default:
                        assert(0);
                        break;
                }
            } else {
                assert(pin_type == pin_type_digital_output);
                if (value) {
                    PTP |= 1<<offset;
                } else {
                    PTP &= ~(1<<offset);
                }
            }
            break;
        case PIN_PS0:
        case PIN_PS1:
        case PIN_PS2:
        case PIN_PS3:
        case PIN_PS4:
        case PIN_PS5:
        case PIN_PS6:
        case PIN_PS7:
            offset = pin_number - PIN_PS0;
            assert(offset < 8);
            if (pin_type == pin_type_uart_output) {
                if (pin_number == PIN_PS1) {
                    pin_uart_tx(0, value);
                } else {
                    assert(pin_number == PIN_PS3);
                    pin_uart_tx(1, value);
                }
            } else {
                assert(pin_type == pin_type_digital_output);
                if (value) {
                    PTS |= 1<<offset;
                } else {
                    PTS &= ~(1<<offset);
                }
            }
            break;
        case PIN_PT0:
        case PIN_PT1:
        case PIN_PT2:
        case PIN_PT3:
        case PIN_PT4:
        case PIN_PT5:
        case PIN_PT6:
        case PIN_PT7:
            offset = pin_number - PIN_PT0;
            assert(offset < 8);
            if (pin_type == pin_type_frequency_output) {
                // hack to work around high minimum frequency
                if (! value) {
                    // disable timer
                    TSCR1 = 0x00;
                } else {
                    switch (offset) {
                        case 0:
                            TC0 = value;
                            break;
                        case 1:
                            TC1 = value;
                            break;
                        case 2:
                            TC2 = value;
                            break;
                        case 3:
                            TC3 = value;
                            break;
                        case 4:
                            TC4 = value;
                            break;
                        case 5:
                            TC5 = value;
                            break;
                        case 6:
                            TC6 = value;
                            break;
                        case 7:
                            // NULL
                            break;
                        default:
                            assert(0);
                    }
                    TC7 = value;
                    
                    // enable timer
                    TSCR1 = 0x80;
                }
            } else {
                assert(pin_type == pin_type_digital_output);
                if (value) {
                    PTT |= 1<<offset;
                } else {
                    PTT &= ~(1<<offset);
                }
            }
            break;
        default:
            assert(0);
            break;
    }
#elif PIC32
    // set the PIC32 pin to value
    switch (pin_number) {
#ifdef PIC32PORTA
        case PIN_RA0:
        case PIN_RA1:
        case PIN_RA2:
        case PIN_RA3:
        case PIN_RA4:
        case PIN_RA5:
        case PIN_RA6:
        case PIN_RA7:
        case PIN_RA9:
        case PIN_RA10:
        case PIN_RA14:
        case PIN_RA15:
            offset = pin_number - PIN_RA0;
            assert(offset < 16);
            assert(pin_type == pin_type_digital_output);
            if (value) {
                LATASET = 1<<offset;
            } else {
                LATACLR = 1<<offset;
            }
            break;
#endif
        case PIN_AN0:
        case PIN_AN1:
        case PIN_AN2:
        case PIN_AN3:
        case PIN_AN4:
        case PIN_AN5:
        case PIN_AN6:
        case PIN_AN7:
        case PIN_AN8:
        case PIN_AN9:
        case PIN_AN10:
        case PIN_AN11:
        case PIN_AN12:
        case PIN_AN13:
        case PIN_AN14:
        case PIN_AN15:
#if PIC32PPS
            break;
#else
            offset = pin_number - PIN_AN0;
            assert(offset < 16);
            assert(pin_type == pin_type_digital_output);
            if (value) {
                LATBSET = 1<<offset;
            } else {
                LATBCLR = 1<<offset;
            }
            break;
        case PIN_RC1:
        case PIN_RC2:
        case PIN_RC3:
        case PIN_RC4:
        case PIN_RC13:
        case PIN_RC14:
            offset = pin_number - PIN_RC0;
            assert(offset < 16);
            assert(pin_type == pin_type_digital_output);
            if (value) {
                LATCSET = 1<<offset;
            } else {
                LATCCLR = 1<<offset;
            }
            break;
        case PIN_RD0:
        case PIN_RD1:
        case PIN_RD2:
        case PIN_RD3:
        case PIN_RD4:
        case PIN_RD5:
        case PIN_RD6:
        case PIN_RD7:
        case PIN_RD8:
        case PIN_RD9:
        case PIN_RD10:
        case PIN_RD11:
        case PIN_RD12:
        case PIN_RD13:
        case PIN_RD14:
        case PIN_RD15:
            offset = pin_number - PIN_RD0;
            assert(offset < 16);
            if (pin_type == pin_type_analog_output || pin_type == pin_type_servo_output) {
                if (pin_type == pin_type_servo_output) {
                    value = value*SERVO_MOD/SERVO_MAX;
                }
                if (offset == 0) {
                    OC1CONCLR = _OC1CON_ON_MASK;
                    OC1R = 0;
                    OC1RS = value;
                    OC1CON = _OC1CON_ON_MASK|(6<<_OC1CON_OCM0_POSITION);
                } else if (offset == 1) {
                    OC2CONCLR = _OC2CON_ON_MASK;
                    OC2R = 0;
                    OC2RS = value;
                    OC2CON = _OC2CON_ON_MASK|(6<<_OC2CON_OCM0_POSITION);
                } else if (offset == 2) {
                    OC3CONCLR = _OC3CON_ON_MASK;
                    OC3R = 0;
                    OC3RS = value;
                    OC3CON = _OC3CON_ON_MASK|(6<<_OC3CON_OCM0_POSITION);
                } else if (offset == 3) {
                    OC4CONCLR = _OC4CON_ON_MASK;
                    OC4R = 0;
                    OC4RS = value;
                    OC4CON = _OC4CON_ON_MASK|(6<<_OC4CON_OCM0_POSITION);
                } else {
                    assert(offset == 4);
                    OC5CONCLR = _OC5CON_ON_MASK;
                    OC5R = 0;
                    OC5RS = value;
                    OC5CON = _OC5CON_ON_MASK|(6<<_OC5CON_OCM0_POSITION);
                }                
            } else if (pin_type == pin_type_frequency_output) {
                // configure timer 3 for frequency generation
                T3CONCLR = _T3CON_ON_MASK;
                TMR3 = 0;
                PR3 = value;
                if (offset == 0) {
                    OC1CONCLR = _OC1CON_ON_MASK;
                    OC1R = 0;
                    OC1RS = value;
                    // hack to work around high minimum frequency
                    if (value) {
                        OC1CON = _OC1CON_ON_MASK|_OC1CON_OCTSEL_MASK|(3<<_OC1CON_OCM0_POSITION);
                    }
                } else if (offset == 1) {
                    OC2CONCLR = _OC2CON_ON_MASK;
                    OC2R = 0;
                    OC2RS = value;
                    // hack to work around high minimum frequency
                    if (value) {
                        OC2CON = _OC2CON_ON_MASK|_OC2CON_OCTSEL_MASK|(3<<_OC2CON_OCM0_POSITION);
                    }
                } else if (offset == 2) {
                    OC3CONCLR = _OC3CON_ON_MASK;
                    OC3R = 0;
                    OC3RS = value;
                    // hack to work around high minimum frequency
                    if (value) {
                        OC3CON = _OC3CON_ON_MASK|_OC3CON_OCTSEL_MASK|(3<<_OC3CON_OCM0_POSITION);
                    }
                } else if (offset == 3) {
                    OC4CONCLR = _OC4CON_ON_MASK;
                    OC4R = 0;
                    OC4RS = value;
                    // hack to work around high minimum frequency
                    if (value) {
                        OC4CON = _OC4CON_ON_MASK|_OC4CON_OCTSEL_MASK|(3<<_OC4CON_OCM0_POSITION);
                    }
                } else {
                    assert(offset == 4);
                    OC5CONCLR = _OC5CON_ON_MASK;
                    OC5R = 0;
                    OC5RS = value;
                    // hack to work around high minimum frequency
                    if (value) {
                        OC5CON = _OC5CON_ON_MASK|_OC5CON_OCTSEL_MASK|(3<<_OC5CON_OCM0_POSITION);
                    }
                }                
                // set timer prescale to 64
                T3CON = _T3CON_ON_MASK|(6<<_T3CON_TCKPS0_POSITION);
                assert(FREQ_PRESCALE == 64);
            } else {
                assert(pin_type == pin_type_digital_output);
                if (value) {
                    LATDSET = 1<<offset;
                } else {
                    LATDCLR = 1 << offset;
                }
            }
            break;
        case PIN_RE0:
        case PIN_RE1:
        case PIN_RE2:
        case PIN_RE3:
        case PIN_RE4:
        case PIN_RE5:
        case PIN_RE6:
        case PIN_RE7:
        case PIN_RE8:
        case PIN_RE9:
            offset = pin_number - PIN_RE0;
            assert(offset < 16);
            assert(pin_type == pin_type_digital_output);
            if (value) {
                LATESET = 1<<offset;
            } else {
                LATECLR = 1<<offset;
            }
            break;
        case PIN_RF0:
        case PIN_RF1:
        case PIN_RF2:
        case PIN_RF3:
        case PIN_RF4:
        case PIN_RF5:
        case PIN_RF6:
        case PIN_RF8:
        case PIN_RF12:
        case PIN_RF13:
            offset = pin_number - PIN_RF0;
            assert(offset < 16);
            if (pin_type == pin_type_uart_output) {
                assert(pin_number == PIN_RF5);
                pin_uart_tx(1, value);
            } else {
                assert(pin_type == pin_type_digital_output);
                if (value) {
                    LATFSET = 1<<offset;
                } else {
                    LATFCLR = 1<<offset;
                }
            }
            break;
        case PIN_RG0:
        case PIN_RG1:
        case PIN_RG6:
        case PIN_RG7:
        case PIN_RG8:
        case PIN_RG9:
        case PIN_RG12:
        case PIN_RG13:
        case PIN_RG14:
        case PIN_RG15:
            offset = pin_number - PIN_RG0;
            assert(offset < 16);
            assert(pin_type == pin_type_digital_output);
            if (value) {
                LATGSET = 1<<offset;
            } else {
                LATGCLR = 1<<offset;
            }
            break;
#endif
        default:
            assert(0);
            break;
    }
#else
#error
#endif // MCF52221 || MCF52233 || MCF52259 || MCF5211
#endif // ! STICK_GUEST

    // if setting to 0, then enable the pin driver after the pin data value has been set to 0.  This prevents the processor from
    // driving a 1 (which the pin's latch may held at the start of this function).
    if ((! value) && (pin_qual & 1<<pin_qual_open_drain)) {
        assert(pin_type == pin_type_digital_output);
        pin_declare_internal(pin_number, pin_type_digital_output, pin_qual, true, false);
    }
}
#if MC9S08QE128 || MC9S12DT256 || MC9S12DP512
#pragma CODE_SEG DEFAULT
#endif

// this function gets a pin variable!
int32
pin_get(IN int pin_number, IN int pin_type, IN int pin_qual)
{
#if ! STICK_GUEST && ! SHRINK
#if MCF51JM128 || MCF51CN128 || MCF51QE128 || MC9S08QE128 || MC9S12DT256 || MC9S12DP512
    uint32 adc;
#endif
    int32 value;
    uint32 offset;
    
    value = 0;

    if (pin_number == PIN_UNASSIGNED) {
        return value;
    }

#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    // get the value of the MCF52221/MCF52233/MCF52259/MCF5211 pin
    switch (pin_number) {
        case PIN_DTIN0:
        case PIN_DTIN1:
        case PIN_DTIN2:
        case PIN_DTIN3:
            offset = pin_number - PIN_DTIN0;
            assert(offset < 4);
            if (pin_type == pin_type_analog_output) {
                value = (pin_analog-pwmget(offset*2));
            } else if (pin_type == pin_type_servo_output) {
                value = (SERVO_MOD-pwmget(offset*2))*SERVO_MAX/SERVO_MOD;
            } else if (pin_type == pin_type_frequency_output) {
                if (MCF_DTIM_DTRR(offset) == -1) {
                    value = 0;
                } else {
                    value = bus_frequency/(MCF_DTIM_DTRR(offset) + 1);
                }                        
            } else if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_tc, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (MCF_GPIO_SETTC & (1 << offset));
            }
            break;
        case PIN_QSPI_DOUT:
        case PIN_QSPI_DIN:
        case PIN_QSPI_CLK:
        case PIN_QSPI_CS0:
#if MCF52259
        case PIN_QSPI_CS2:
        case PIN_QSPI_CS3:
#endif
            offset = pin_number - PIN_QSPI_DOUT;
            assert(offset < 8);
            if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_qs, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (MCF_GPIO_SETQS & (1 << offset));
            }
            break;
#if MCF52259
        case PIN_FEC_COL:
        case PIN_FEC_CRS:
        case PIN_FEC_RXCLK:
        case PIN_FEC_RXD0:
        case PIN_FEC_RXD1:
        case PIN_FEC_RXD2:
        case PIN_FEC_RXD3:
        case PIN_FEC_RXDV:
            offset = pin_number - PIN_FEC_COL;
            assert(offset < 8);
            if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_ti, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (MCF_GPIO_SETTI & (1 << offset));
            }
            break;
        case PIN_FEC_RXER:
        case PIN_FEC_TXCLK:
        case PIN_FEC_TXD0:
        case PIN_FEC_TXD1:
        case PIN_FEC_TXD2:
        case PIN_FEC_TXD3:
        case PIN_FEC_TXEN:
        case PIN_FEC_TXER:
            offset = pin_number - PIN_FEC_RXER;
            assert(offset < 8);
            if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_tj, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (MCF_GPIO_SETTJ & (1 << offset));
            }
            break;
        case PIN_UTXD2:
        case PIN_URXD2:
        case PIN_RTS2:
        case PIN_CTS2:
            offset = pin_number - PIN_UTXD2;
            assert(offset < 4);
            if (pin_type == pin_type_uart_input) {
                assert(pin_number == PIN_URXD2);
                value = pin_uart_rx(2);
            } else if (pin_type == pin_type_uart_output) {
                value = pin_uart_tx_empty(2)?0:ulasttx[2];
            } else if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_uc, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (MCF_GPIO_SETUC & (1 << offset));
            }
            break;
#endif
        case PIN_UTXD1:
        case PIN_URXD1:
        case PIN_RTS1:
        case PIN_CTS1:
            offset = pin_number - PIN_UTXD1;
            assert(offset < 4);
            if (pin_type == pin_type_uart_input) {
                assert(pin_number == PIN_URXD1);
                value = pin_uart_rx(1);
            } else if (pin_type == pin_type_uart_output) {
                value = pin_uart_tx_empty(1)?0:ulasttx[1];
            } else if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_ub, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (MCF_GPIO_SETUB & (1 << offset));
            }
            break;
        case PIN_UTXD0:
        case PIN_URXD0:
        case PIN_RTS0:
        case PIN_CTS0:
            offset = pin_number - PIN_UTXD0;
            assert(offset < 4);
            if (pin_type == pin_type_uart_input) {
                assert(pin_number == PIN_URXD0);
                value = pin_uart_rx(0);
            } else if (pin_type == pin_type_uart_output) {
                value = pin_uart_tx_empty(0)?0:ulasttx[0];
            } else if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_ua, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (MCF_GPIO_SETUA & (1 << offset));
            }
            break;
        case PIN_AN0:
        case PIN_AN1:
        case PIN_AN2:
        case PIN_AN3:
        case PIN_AN4:
        case PIN_AN5:
        case PIN_AN6:
        case PIN_AN7:
            offset = pin_number - PIN_AN0;
            assert(offset < 8);
            if (pin_type == pin_type_analog_input) {
                value = adc_get_value(offset, pin_qual);
            } else if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_an, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (MCF_GPIO_SETAN & (1 << offset));
            }
            break;
        case PIN_IRQ1:
        case PIN_IRQ2:
        case PIN_IRQ3:
        case PIN_IRQ4:
        case PIN_IRQ5:
        case PIN_IRQ6:
        case PIN_IRQ7:
            offset = pin_number - PIN_IRQ0;
            assert(offset < 8);
            if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_nq, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (MCF_GPIO_SETNQ & (1 << offset));
            }
            break;
#if MCF52233
        case PIN_IRQ11:
            offset = pin_number - PIN_IRQ8;
            assert(offset < 8);
            if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_gp, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (MCF_GPIO_SETGP & (1 << offset));
            }
            break;
#endif
#if MCF52233 || MCF52259 || MCF5211
        case PIN_GPT0:
        case PIN_GPT1:
        case PIN_GPT2:
        case PIN_GPT3:
            offset = pin_number - PIN_GPT0;
            assert(offset < 4);            
            if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_ta, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (MCF_GPIO_SETTA & (1 << offset));
            }
            break;
#endif
        case PIN_SCL:
        case PIN_SDA:
            offset = pin_number - PIN_SCL;
            assert(offset < 2);
            if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_as, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (MCF_GPIO_SETAS & (1 << offset));
            }
            break;
        default:
            assert(0);
            break;
    }
#elif MCF51JM128
    // get the value of the MCF51JM128 pin
    switch (pin_number) {
        case PIN_PTA0:
        case PIN_PTA1:
        case PIN_PTA2:
        case PIN_PTA3:
        case PIN_PTA4:
        case PIN_PTA5:
            offset = pin_number - PIN_PTA0;
            assert(offset < 6);
            if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_a, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (PTAD & (1<<offset));
            }
            break;
        case PIN_PTB0:  // adc0
        case PIN_PTB1:  // adc1
        case PIN_PTB2:  // adc2
        case PIN_PTB3:  // adc3
        case PIN_PTB4:  // adc4
        case PIN_PTB5:  // adc5
        case PIN_PTB6:  // adc6
        case PIN_PTB7:  // adc7
            offset = pin_number - PIN_PTB0;
            assert(offset < 8);
            if (pin_type == pin_type_analog_input) {
                adc = offset;
                assert(adc >= 0 && adc < 8);
                value = adc_get_value(adc, pin_qual);
            } else if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_b, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (PTBD & (1<<offset));
            }
            break;
        case PIN_PTC0:
        case PIN_PTC1:
        case PIN_PTC2:
        case PIN_PTC3:  // u2tx
        case PIN_PTC4:
        case PIN_PTC5:  // u2rx
        case PIN_PTC6:
            offset = pin_number - PIN_PTC0;
            assert(offset < 8);
            if (pin_type == pin_type_uart_input) {
                assert(offset == 5);
                value = pin_uart_rx(1);
            } else if (pin_type == pin_type_uart_output) {
                assert(offset == 3);
                value = pin_uart_tx_empty(1)?0:ulasttx[1];
            } else if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_c, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (PTCD & (1<<offset));
            }
            break;
        case PIN_PTD0:  // adc8
        case PIN_PTD1:  // adc9
        case PIN_PTD2:
        case PIN_PTD3:  // adc10
        case PIN_PTD4:  // adc11
        case PIN_PTD5:
        case PIN_PTD6:
        case PIN_PTD7:
            offset = pin_number - PIN_PTD0;
            assert(offset < 8);
            if (pin_type == pin_type_analog_input) {
                assert(offset < 2 || offset >= 3 && offset < 5);
                adc = (offset<2)?(8+offset):(8+offset-1);
                assert(adc >= 8 && adc < 12);
                value = adc_get_value(adc, pin_qual);
            } else if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_d, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (PTDD & (1<<offset));
            }
            break;
        case PIN_PTE0:  // u1tx
        case PIN_PTE1:  // u1rx
        case PIN_PTE2:
        case PIN_PTE3:
        case PIN_PTE4:
        case PIN_PTE5:
        case PIN_PTE6:
        case PIN_PTE7:
            offset = pin_number - PIN_PTE0;
            assert(offset < 8);
            if (pin_type == pin_type_analog_output || pin_type == pin_type_servo_output || pin_type == pin_type_frequency_output) {
                assert(offset >= 2 && offset < 4);
                // read the pwm/frequency
                if (offset == 2) {
                    value = (TPM1C0VH << 8) | TPM1C0VL;
                } else {
                    value = (TPM1C1VH << 8) | TPM1C1VL;
                }
                
                if (pin_type == pin_type_frequency_output) {
                    if (! value) {
                        value = 0x10000;
                    }
                    value = bus_frequency/FREQ_PRESCALE/(value+1)/2;
                } else if (pin_type == pin_type_servo_output) {
                    value = value*SERVO_MAX/SERVO_MOD;
                }
            } else if (pin_type == pin_type_uart_input) {
                assert(offset == 1);
                value = pin_uart_rx(0);
            } else if (pin_type == pin_type_uart_output) {
                assert(offset == 0);
                value = pin_uart_tx_empty(0)?0:ulasttx[0];
            } else if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_e, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (PTED & (1<<offset));
            }
            break;
        case PIN_PTF0:
        case PIN_PTF1:
        case PIN_PTF2:
        case PIN_PTF3:
        case PIN_PTF4:
        case PIN_PTF5:
        case PIN_PTF6:
        case PIN_PTF7:
            offset = pin_number - PIN_PTF0;
            assert(offset < 8);
            if (pin_type == pin_type_analog_output || pin_type == pin_type_servo_output || pin_type == pin_type_frequency_output) {
                assert(offset < 6);
                // read the pwm/frequency
                if (offset == 0) {
                    value = (TPM1C2VH << 8) | TPM1C2VL;
                } else if (offset == 1) {
                    value = (TPM1C3VH << 8) | TPM1C3VL;
                } else if (offset == 2) {
                    value = (TPM1C4VH << 8) | TPM1C4VL;
                } else if (offset == 3) {
                    value = (TPM1C5VH << 8) | TPM1C5VL;
                } else if (offset == 4) {
                    value = (TPM2C0VH << 8) | TPM2C0VL;
                } else {
                    value = (TPM2C1VH << 8) | TPM2C1VL;
                }
                
                if (pin_type == pin_type_frequency_output) {
                    if (! value) {
                        value = 0x10000;
                    }
                    value = bus_frequency/FREQ_PRESCALE/(value+1)/2;
                } else if (pin_type == pin_type_servo_output) {
                    value = value*SERVO_MAX/SERVO_MOD;
                }
            } else if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_f, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (PTFD & (1<<offset));
            }
            break;
        case PIN_PTG0:
        case PIN_PTG1:
        case PIN_PTG2:
        case PIN_PTG3:
            offset = pin_number - PIN_PTG0;
            assert(offset < 4);
            if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_g, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (PTGD & (1<<offset));
            }
            break;
        default:
            assert(0);
            break;
    }
#elif MCF51CN128
    // get the value of the MCF51CN128 pin
    switch (pin_number) {
        case PIN_PTA0:
        case PIN_PTA1:
        case PIN_PTA2:
        case PIN_PTA3:  // u3tx
        case PIN_PTA4:  // u3rx
        case PIN_PTA5:
        case PIN_PTA6:
        case PIN_PTA7:
            offset = pin_number - PIN_PTA0;
            assert(offset < 8);
            if (pin_type == pin_type_uart_input) {
                assert(offset == 4);
                value = pin_uart_rx(2);
            } else if (pin_type == pin_type_uart_output) {
                assert(offset == 3);
                value = pin_uart_tx_empty(2)?0:ulasttx[2];
            } else if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_a, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (PTAD & (1<<offset));
            }
            break;
        case PIN_PTB0:
        case PIN_PTB1:
        case PIN_PTB2:
        case PIN_PTB3:
        case PIN_PTB4:
        case PIN_PTB5:
        case PIN_PTB6:  // tpm2ch0
        case PIN_PTB7:  // tpm2ch1
            offset = pin_number - PIN_PTB0;
            assert(offset < 8);
            if (pin_type == pin_type_analog_output || pin_type == pin_type_servo_output || pin_type == pin_type_frequency_output) {
                assert(offset >= 6 && offset < 8);
                // read the pwm/frequency
                if (offset == 6) {
                    value = (TPM2C0VH << 8) | TPM2C0VL;
                } else {
                    assert(offset == 7);
                    value = (TPM2C1VH << 8) | TPM2C1VL;
                }
                
                if (pin_type == pin_type_frequency_output) {
                    if (! value) {
                        value = 0x10000;
                    }
                    value = bus_frequency/FREQ_PRESCALE/(value+1)/2;
                } else if (pin_type == pin_type_servo_output) {
                    value = value*SERVO_MAX/SERVO_MOD;
                }
            } else if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_b, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (PTBD & (1<<offset));
            }
            break;
        case PIN_PTC0:  // tpm2ch2
        case PIN_PTC1:
        case PIN_PTC2:
        case PIN_PTC3:
        case PIN_PTC4:  // adc11
        case PIN_PTC5:  // adc10
        case PIN_PTC6:  // adc9
        case PIN_PTC7:  // adc8
            offset = pin_number - PIN_PTC0;
            assert(offset < 8);
            if (pin_type == pin_type_analog_input) {
                assert(offset < 8);
                adc = 15-offset;
                assert(adc >= 8 && adc < 12);
                value = adc_get_value(adc, pin_qual);
            } else if (pin_type == pin_type_analog_output || pin_type == pin_type_servo_output || pin_type == pin_type_frequency_output) {
                assert(offset == 0);
                // read the pwm/frequency
                assert(offset == 0);
                value = (TPM2C2VH << 8) | TPM2C2VL;
                
                if (pin_type == pin_type_frequency_output) {
                    if (! value) {
                        value = 0x10000;
                    }
                    value = bus_frequency/FREQ_PRESCALE/(value+1)/2;
                } else if (pin_type == pin_type_servo_output) {
                    value = value*SERVO_MAX/SERVO_MOD;
                }
            } else if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_c, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (PTCD & (1<<offset));
            }
            break;
        case PIN_PTD0:  // adc7, u1tx
        case PIN_PTD1:  // adc6, u1rx
        case PIN_PTD2:  // adc5, u2tx
        case PIN_PTD3:  // adc4, u2rx
        case PIN_PTD4:
        case PIN_PTD5:
        case PIN_PTD6:
        case PIN_PTD7:  // adc3
            offset = pin_number - PIN_PTD0;
            assert(offset < 8);
            if (pin_type == pin_type_analog_input) {
                assert(offset < 4 || offset == 7);
                adc = (offset<4)?(7-offset):3;
                assert(adc >= 3 && adc < 8);
                value = adc_get_value(adc, pin_qual);
            } else if (pin_type == pin_type_uart_input) {
                assert(offset == 1 || offset == 3);
                if (offset == 1) {
                    value = pin_uart_rx(0);
                } else {
                    value = pin_uart_rx(1);
                }
            } else if (pin_type == pin_type_uart_output) {
                assert(offset == 0 || offset == 2);
                if (offset == 0) {
                    value = pin_uart_tx_empty(0)?0:ulasttx[0];
                } else {
                    value = pin_uart_tx_empty(1)?0:ulasttx[1];
                }
            } else if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_d, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (PTDD & (1<<offset));
            }
            break;
        case PIN_PTE0:  // adc2
        case PIN_PTE1:  // adc1
        case PIN_PTE2:  // adc0
        case PIN_PTE3:  // tpm1ch0
        case PIN_PTE4:  // tpm1ch1
        case PIN_PTE5:  // tpm1ch2
        case PIN_PTE6:
        case PIN_PTE7:
            offset = pin_number - PIN_PTE0;
            assert(offset < 8);
            if (pin_type == pin_type_analog_input) {
                assert(offset < 3);
                adc = 2-offset;
                assert(adc < 3);
                value = adc_get_value(adc, pin_qual);
            } else if (pin_type == pin_type_analog_output || pin_type == pin_type_servo_output || pin_type == pin_type_frequency_output) {
                assert(offset >= 3 && offset < 6);
                // read the pwm/frequency
                if (offset == 3) {
                    value = (TPM1C0VH << 8) | TPM1C0VL;
                } else if (offset == 4) {
                    value = (TPM1C1VH << 8) | TPM1C1VL;
                } else {
                    assert(offset == 5);
                    value = (TPM1C2VH << 8) | TPM1C2VL;
                }
                
                if (pin_type == pin_type_frequency_output) {
                    if (! value) {
                        value = 0x10000;
                    }
                    value = bus_frequency/FREQ_PRESCALE/(value+1)/2;
                } else if (pin_type == pin_type_servo_output) {
                    value = value*SERVO_MAX/SERVO_MOD;
                }
            } else if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_e, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (PTED & (1<<offset));
            }
            break;
        case PIN_PTF0:
        case PIN_PTF1:
        case PIN_PTF2:
        case PIN_PTF3:
        case PIN_PTF4:
        case PIN_PTF5:
        case PIN_PTF6:
        case PIN_PTF7:
            offset = pin_number - PIN_PTF0;
            assert(offset < 8);
            if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_f, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (PTFD & (1<<offset));
            }
            break;
        case PIN_PTG0:
        case PIN_PTG1:
        case PIN_PTG2:
        case PIN_PTG3:
        case PIN_PTG4:
        case PIN_PTG5:
        case PIN_PTG6:
        case PIN_PTG7:
            offset = pin_number - PIN_PTG0;
            assert(offset < 8);
            if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_g, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (PTGD & (1<<offset));
            }
            break;
        case PIN_PTH0:
        case PIN_PTH1:
        case PIN_PTH2:
        case PIN_PTH3:
        case PIN_PTH4:
        case PIN_PTH5:
        case PIN_PTH6:
        case PIN_PTH7:
            offset = pin_number - PIN_PTH0;
            assert(offset < 8);
            if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_h, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (PTHD & (1<<offset));
            }
            break;
        case PIN_PTJ0:
        case PIN_PTJ1:
        case PIN_PTJ2:
        case PIN_PTJ3:
        case PIN_PTJ4:
        case PIN_PTJ5:
            offset = pin_number - PIN_PTJ0;
            assert(offset < 6);
            if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_j, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (PTJD & (1<<offset));
            }
            break;
        default:
            assert(0);
            break;
    }
#elif MCF51QE128 || MC9S08QE128
    // get the value of the MCF51QE128/MC9S08QE128 pin
    switch (pin_number) {
        case PIN_PTA0:  // adc0, tpm1ch0
        case PIN_PTA1:  // adc1, tmp2ch0
        case PIN_PTA2:  // adc2
        case PIN_PTA3:  // adc3
        case PIN_PTA4:
        case PIN_PTA5:
        case PIN_PTA6:  // adc8, tpm1ch2
        case PIN_PTA7:  // adc9, tpm2ch2
            offset = pin_number - PIN_PTA0;
            assert(offset < 8);
            if (pin_type == pin_type_analog_input) {
                adc = (offset<4)?(offset):(8+offset-6);
                assert(adc < 4 || adc >= 8 && adc < 10);
                value = adc_get_value(adc, pin_qual);
            } else if (pin_type == pin_type_analog_output || pin_type == pin_type_servo_output || pin_type == pin_type_frequency_output) {
                assert(offset < 2 || offset >= 6);
                // read the pwm/frequency
                if (offset == 0) {
                    value = (TPM1C0VH << 8) | TPM1C0VL;
                } else if (offset == 1) {
                    value = (TPM2C0VH << 8) | TPM2C0VL;
                } else if (offset == 6) {
                    value = (TPM1C2VH << 8) | TPM1C2VL;
                } else {
                    assert(offset == 7);
                    value = (TPM2C2VH << 8) | TPM2C2VL;
                }
                
                if (pin_type == pin_type_frequency_output) {
                    if (! value) {
                        value = 0x10000;
                    }
                    value = bus_frequency/FREQ_PRESCALE/(value+1)/2;
                } else if (pin_type == pin_type_servo_output) {
                    value = value*SERVO_MAX/SERVO_MOD;
                }
            } else if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_a, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (PTAD & (1<<offset));
            }
            break;
        case PIN_PTB0:  // adc4, u1rx
        case PIN_PTB1:  // adc5, u1tx
        case PIN_PTB2:  // adc6
        case PIN_PTB3:  // adc7
        case PIN_PTB4:  // tpm2ch1
        case PIN_PTB5:  // tpm1ch1
        case PIN_PTB6:
        case PIN_PTB7:
            offset = pin_number - PIN_PTB0;
            assert(offset < 8);
            if (pin_type == pin_type_analog_input) {
                assert(offset < 4);
                adc = 4+offset;
                assert(adc >= 4 && adc < 8);
                value = adc_get_value(adc, pin_qual);
            } else if (pin_type == pin_type_uart_input) {
                assert(offset == 0);
                value = pin_uart_rx(0);
            } else if (pin_type == pin_type_uart_output) {
                assert(offset == 1);
                value = pin_uart_tx_empty(0)?0:ulasttx[0];
            } else if (pin_type == pin_type_analog_output || pin_type == pin_type_servo_output || pin_type == pin_type_frequency_output) {
                assert(offset >= 4 || offset < 6);
                // read the pwm/frequency
                if (offset == 4) {
                    value = (TPM2C1VH << 8) | TPM2C1VL;
                } else {
                    assert(offset == 5);
                    value = (TPM1C1VH << 8) | TPM1C1VL;
                }
                
                if (pin_type == pin_type_frequency_output) {
                    if (! value) {
                        value = 0x10000;
                    }
                    value = bus_frequency/FREQ_PRESCALE/(value+1)/2;
                } else if (pin_type == pin_type_servo_output) {
                    value = value*SERVO_MAX/SERVO_MOD;
                }
            } else if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_b, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (PTBD & (1<<offset));
            }
            break;
        case PIN_PTC0:  // tpm3ch0
        case PIN_PTC1:  // tpm3ch1
        case PIN_PTC2:  // tpm3ch2
        case PIN_PTC3:  // tpm3ch3
        case PIN_PTC4:  // tpm3ch4
        case PIN_PTC5:  // tpm3ch5
        case PIN_PTC6:  // u2rx
        case PIN_PTC7:  // u2tx
            offset = pin_number - PIN_PTC0;
            assert(offset < 8);
            if (pin_type == pin_type_uart_input) {
                assert(offset == 6);
                value = pin_uart_rx(1);
            } else if (pin_type == pin_type_uart_output) {
                assert(offset == 7);
                value = pin_uart_tx_empty(1)?0:ulasttx[1];
            } else if (pin_type == pin_type_analog_output || pin_type == pin_type_servo_output || pin_type == pin_type_frequency_output) {
                assert(offset < 6);
                // read the pwm/frequency
                if (offset == 0) {
                    value = (TPM3C0VH << 8) | TPM3C0VL;
                } else if (offset == 1) {
                    value = (TPM3C1VH << 8) | TPM3C1VL;
                } else if (offset == 2) {
                    value = (TPM3C2VH << 8) | TPM3C2VL;
                } else if (offset == 3) {
                    value = (TPM3C3VH << 8) | TPM3C3VL;
                } else if (offset == 4) {
                    value = (TPM3C4VH << 8) | TPM3C4VL;
                } else {
                    assert(offset == 5);
                    value = (TPM3C5VH << 8) | TPM3C5VL;
                }
                
                if (pin_type == pin_type_frequency_output) {
                    if (! value) {
                        value = 0x10000;
                    }
                    value = bus_frequency/FREQ_PRESCALE/(value+1)/2;
                } else if (pin_type == pin_type_servo_output) {
                    value = value*SERVO_MAX/SERVO_MOD;
                }
            } else if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_c, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (PTCD & (1<<offset));
            }
            break;
        case PIN_PTD0:
        case PIN_PTD1:
        case PIN_PTD2:
        case PIN_PTD3:
        case PIN_PTD4:
        case PIN_PTD5:
        case PIN_PTD6:
        case PIN_PTD7:
            offset = pin_number - PIN_PTD0;
            assert(offset < 8);
            if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_d, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (PTDD & (1<<offset));
            }
            break;
        case PIN_PTE0:
        case PIN_PTE1:
        case PIN_PTE2:
        case PIN_PTE3:
        case PIN_PTE4:
        case PIN_PTE5:
        case PIN_PTE6:
        case PIN_PTE7:
            offset = pin_number - PIN_PTE0;
            assert(offset < 8);
            if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_e, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (PTED & (1<<offset));
            }
            break;
        case PIN_PTF0:  // adc10
        case PIN_PTF1:  // adc11
        case PIN_PTF2:  // adc12
        case PIN_PTF3:  // adc13
        case PIN_PTF4:  // adc14
        case PIN_PTF5:  // adc15
        case PIN_PTF6:  // adc16
        case PIN_PTF7:  // adc17
            offset = pin_number - PIN_PTF0;
            assert(offset < 8);
            if (pin_type == pin_type_analog_input) {
                assert(offset < 8);
                adc = 10+offset;
                assert(adc >= 10 && adc < 18);
                value = adc_get_value(adc, pin_qual);
            } else if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_f, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (PTFD & (1<<offset));
            }
            break;
        case PIN_PTG0:
        case PIN_PTG1:
        case PIN_PTG2:  // adc18
        case PIN_PTG3:  // adc19
            offset = pin_number - PIN_PTG0;
            assert(offset < 4);
            if (pin_type == pin_type_analog_input) {
                assert(offset >= 2);
                adc = 16+offset;
                assert(adc >= 18 && adc < 20);
                value = adc_get_value(adc, pin_qual);
            } else if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_g, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (PTGD & (1<<offset));
            }
            break;
        default:
            assert(0);
            break;
    }
#elif MC9S12DT256 || MC9S12DP512
    // get the value of the MC9S12DT256/MC9S12DP512 pin
    switch (pin_number) {
        case PIN_PAD00:
        case PIN_PAD01:
        case PIN_PAD02:
        case PIN_PAD03:
        case PIN_PAD04:
        case PIN_PAD05:
        case PIN_PAD06:
        case PIN_PAD07:
            offset = pin_number - PIN_PAD00;
            assert(offset < 8);
            if (pin_type == pin_type_analog_input) {
                adc = offset;
                value = adc_get_value(adc, pin_qual);
            } else if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_ad0, offset);
            } else {
                assert(pin_type == pin_type_digital_input);
                value = (PORTAD0 & (1<<offset));
            }
            break;
        case PIN_PAD08:
        case PIN_PAD09:
        case PIN_PAD10:
        case PIN_PAD11:
        case PIN_PAD12:
        case PIN_PAD13:
        case PIN_PAD14:
        case PIN_PAD15:
            offset = pin_number - PIN_PAD08;
            assert(offset < 8);
            if (pin_type == pin_type_analog_input) {
                adc = 8+offset;
                value = adc_get_value(adc, pin_qual);
            } else if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_ad1, offset);
            } else {
                assert(pin_type == pin_type_digital_input);
                value = (PORTAD1 & (1<<offset));
            }
            break;
        case PIN_PA0:
        case PIN_PA1:
        case PIN_PA2:
        case PIN_PA3:
        case PIN_PA4:
        case PIN_PA5:
        case PIN_PA6:
        case PIN_PA7:
            offset = pin_number - PIN_PA0;
            assert(offset < 8);
            if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_a, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (PORTA & (1<<offset));
            }
            break;
        case PIN_PB0:
        case PIN_PB1:
        case PIN_PB2:
        case PIN_PB3:
        case PIN_PB4:
        case PIN_PB5:
        case PIN_PB6:
        case PIN_PB7:
            offset = pin_number - PIN_PB0;
            assert(offset < 8);
            if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_b, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (PORTB & (1<<offset));
            }
            break;
        case PIN_PE0:
        case PIN_PE1:
        case PIN_PE2:
        case PIN_PE3:
        case PIN_PE4:
        case PIN_PE5:
        case PIN_PE6:
        case PIN_PE7:
            offset = pin_number - PIN_PE0;
            assert(offset < 8);
            if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_e, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (PORTE & (1<<offset));
            }
            break;
        case PIN_PJ0:
        case PIN_PJ1:
        case PIN_PJ2:
        case PIN_PJ3:
        case PIN_PJ4:
        case PIN_PJ5:
        case PIN_PJ6:
        case PIN_PJ7:
            offset = pin_number - PIN_PJ0;
            assert(offset < 8);
            if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_j, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (PTIJ & (1<<offset));
            }
            break;
        case PIN_PM0:
        case PIN_PM1:
        case PIN_PM2:
        case PIN_PM3:
        case PIN_PM4:
        case PIN_PM5:
        case PIN_PM6:
        case PIN_PM7:
            offset = pin_number - PIN_PM0;
            assert(offset < 8);
            if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_m, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (PTIM & (1<<offset));
            }
            break;
        case PIN_PP0:
        case PIN_PP1:
        case PIN_PP2:
        case PIN_PP3:
        case PIN_PP4:
        case PIN_PP5:
        case PIN_PP6:
        case PIN_PP7:
            offset = pin_number - PIN_PP0;
            assert(offset < 8);
            if (pin_type == pin_type_analog_output || pin_type == pin_type_servo_output) {
                switch (offset) {
                    case 0:
                        value = PWMDTY0;
                        break;
                    case 1:
                        value = PWMDTY1;
                        break;
                    case 2:
                        value = PWMDTY2;
                        break;
                    case 3:
                        value = PWMDTY3;
                        break;
                    case 4:
                        value = PWMDTY4;
                        break;
                    case 5:
                        value = PWMDTY5;
                        break;
                    case 6:
                        value = PWMDTY6;
                        break;
                    case 7:
                        value = PWMDTY7;
                        break;
                    default:
                        assert(0);
                        break;
                }
                if (pin_type == pin_type_analog_output) {
                    value = (255-value)*pin_analog/255;
                } else {
                    assert(pin_type == pin_type_servo_output);
                    value = (255-value)*SERVO_MAX/255;
                }
            } else if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_p, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (PTIP & (1<<offset));
            }
            break;
        case PIN_PS0:
        case PIN_PS1:
        case PIN_PS2:
        case PIN_PS3:
        case PIN_PS4:
        case PIN_PS5:
        case PIN_PS6:
        case PIN_PS7:
            offset = pin_number - PIN_PS0;
            assert(offset < 8);
            if (pin_type == pin_type_uart_input) {
                if (offset == 0) {
                    value = pin_uart_rx(0);
                } else {
                    assert(offset == 2);
                    value = pin_uart_rx(1);
                }
            } else if (pin_type == pin_type_uart_output) {
                if (offset == 1) {
                    value = pin_uart_tx_empty(0)?0:ulasttx[0];
                } else {
                    assert(offset == 3);
                    value = pin_uart_tx_empty(1)?0:ulasttx[1];
                }
            } else if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_s, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (PTIS & (1<<offset));
            }
            break;        
        case PIN_PT0:
        case PIN_PT1:
        case PIN_PT2:
        case PIN_PT3:
        case PIN_PT4:
        case PIN_PT5:
        case PIN_PT6:
        case PIN_PT7:
            offset = pin_number - PIN_PT0;
            assert(offset < 8);
            if (pin_type == pin_type_frequency_output) {
                if (! TSCR1) {
                    value = 0;
                } else {
                    value = TC7;
                    if (! value) {
                        value = 0x10000;
                    }
                    value = bus_frequency/FREQ_PRESCALE/(value+1)/2;
                }
            } else if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_t, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (PTIT & (1<<offset));
            }
            break;
        default:
            assert(0);
            break;
    }
#elif PIC32
    // get the value of the PIC32 pin
    switch (pin_number) {
#ifdef PIC32PORTA
        case PIN_RA0:
        case PIN_RA1:
        case PIN_RA2:
        case PIN_RA3:
        case PIN_RA4:
        case PIN_RA5:
        case PIN_RA6:
        case PIN_RA7:
        case PIN_RA9:
        case PIN_RA10:
        case PIN_RA14:
        case PIN_RA15:
            offset = pin_number - PIN_RA0;
            assert(offset < 16);
            if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_a, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (PORTA & 1 << offset);
            }
            break;
#endif
        case PIN_AN0:
        case PIN_AN1:
        case PIN_AN2:
        case PIN_AN3:
        case PIN_AN4:
        case PIN_AN5:
        case PIN_AN6:
        case PIN_AN7:
        case PIN_AN8:
        case PIN_AN9:
        case PIN_AN10:
        case PIN_AN11:
        case PIN_AN12:
        case PIN_AN13:
        case PIN_AN14:
        case PIN_AN15:
#if PIC32PPS
            break;
#else
            offset = pin_number - PIN_AN0;
            assert(offset < 16);
            if (pin_type == pin_type_analog_input) {
                value = adc_get_value(offset, pin_qual);
            } else if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_b, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = !! (PORTB & 1 << offset);
            }
            break;
        case PIN_RC1:
        case PIN_RC2:
        case PIN_RC3:
        case PIN_RC4:
        case PIN_RC13:
        case PIN_RC14:
            offset = pin_number - PIN_RC0;
            assert(offset < 16);
            if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_c, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (PORTC & 1 << offset);
            }
            break;
        case PIN_RD0:
        case PIN_RD1:
        case PIN_RD2:
        case PIN_RD3:
        case PIN_RD4:
        case PIN_RD5:
        case PIN_RD6:
        case PIN_RD7:
        case PIN_RD8:
        case PIN_RD9:
        case PIN_RD10:
        case PIN_RD11:
        case PIN_RD12:
        case PIN_RD13:
        case PIN_RD14:
        case PIN_RD15:
            offset = pin_number - PIN_RD0;
            assert(offset < 16);
            if (pin_type == pin_type_analog_output || pin_type == pin_type_servo_output || pin_type == pin_type_frequency_output) {
                if (offset == 0) {
                    value = OC1RS;
                } else if (offset == 1) {
                    value = OC2RS;
                } else if (offset == 2) {
                    value = OC3RS;
                } else if (offset == 3) {
                    value = OC4RS;
                } else {
                    assert(offset == 4);
                    value = OC5RS;
                }
                if (pin_type == pin_type_frequency_output) {
                    // XXX -- if OCxCONCLR is off we should print 0!
                    if (! value) {
                        value = 0x10000;
                    }
                    value = bus_frequency/FREQ_PRESCALE/(value+1)/2;
                } else if (pin_type == pin_type_servo_output) {
                    value = value*SERVO_MAX/SERVO_MOD;
                }
            } else if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_d, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (PORTD & 1<<offset);
            }
            break;
        case PIN_RE0:
        case PIN_RE1:
        case PIN_RE2:
        case PIN_RE3:
        case PIN_RE4:
        case PIN_RE5:
        case PIN_RE6:
        case PIN_RE7:
        case PIN_RE8:
        case PIN_RE9:
            offset = pin_number - PIN_RE0;
            assert(offset < 16);
            if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_e, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (PORTE & 1 << offset);
            }
            break;
        case PIN_RF0:
        case PIN_RF1:
        case PIN_RF2:
        case PIN_RF3:
        case PIN_RF4:
        case PIN_RF5:
        case PIN_RF6:
        case PIN_RF8:
        case PIN_RF12:
        case PIN_RF13:
            offset = pin_number - PIN_RF0;
            assert(offset < 16);
            if (pin_type == pin_type_uart_input) {
                assert(offset == 4);
                value = pin_uart_rx(1);
            } else if (pin_type == pin_type_uart_output) {
                assert(offset == 5);
                value = pin_uart_tx_empty(1)?0:ulasttx[1];
            } else if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_f, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (PORTF & 1 << offset);
            }
            break;
        case PIN_RG0:
        case PIN_RG1:
        case PIN_RG6:
        case PIN_RG7:
        case PIN_RG8:
        case PIN_RG9:
        case PIN_RG12:
        case PIN_RG13:
        case PIN_RG14:
        case PIN_RG15:
            offset = pin_number - PIN_RG0;
            assert(offset < 16);
            if (pin_qual & 1<<pin_qual_debounced) {
                assert(pin_type == pin_type_digital_input);
                value = pin_get_digital_debounced(port_g, offset);
            } else {
                assert(pin_type == pin_type_digital_input || pin_type == pin_type_digital_output);
                value = (PORTG & 1 << offset);
            }
            break;
#endif
        default:
            assert(0);
            break;
    }
#else
#error
#endif

    if (pin_type == pin_type_digital_input || pin_type == pin_type_digital_output) {
        value = !!value;
    }
    
    if (pin_qual & (1<<pin_qual_inverted)) {
        if (pin_type == pin_type_digital_input || pin_type == pin_type_digital_output) {
            value = ! value;
        } else if (pin_type == pin_type_analog_input || pin_type == pin_type_analog_output) {
            value = pin_analog-value;
        } else if (pin_type == pin_type_servo_output) {
            value = SERVO_MAX-value;
        }
    }

    return value;
#else
    return 0;
#endif
}


// XXX -- could we uninitialize on run_clear()?
// N.B. parity: 0 -> even; 1 -> odd; 2 -> none
void
pin_uart_configure(int uart, int baud, int data, byte parity, bool loopback)
{
#if ! STICK_GUEST
    int divisor;

#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    // configure the uart for the requested protocol and speed
    MCF_UART_UCR(uart) = MCF_UART_UCR_RESET_MR|MCF_UART_UCR_TX_DISABLED|MCF_UART_UCR_RX_DISABLED;
    MCF_UART_UCR(uart) = MCF_UART_UCR_RESET_RX;
    MCF_UART_UCR(uart) = MCF_UART_UCR_RESET_TX;

    MCF_UART_UMR(uart)/*1*/ = (parity==0?MCF_UART_UMR_PM_ODD:(parity==1?MCF_UART_UMR_PM_EVEN:MCF_UART_UMR_PM_NONE))|(data-5);
    MCF_UART_UMR(uart)/*2*/ = (loopback?MCF_UART_UMR_CM_LOCAL_LOOP:0)|MCF_UART_UMR_SB_STOP_BITS_1;

    MCF_UART_UCSR(uart) = MCF_UART_UCSR_RCS_SYS_CLK|MCF_UART_UCSR_TCS_SYS_CLK;

    divisor = bus_frequency/baud/16;
    if (divisor >= 0x10000) {
        divisor = 0xffff;
    }
    MCF_UART_UBG1(uart) = (uint8)(divisor/0x100);
    MCF_UART_UBG2(uart) = (uint8)(divisor%0x100);

    MCF_UART_UCR(uart) = MCF_UART_UCR_TX_ENABLED|MCF_UART_UCR_RX_ENABLED;
#elif MCF51JM128 || MCF51CN128 || MCF51QE128 || MC9S08QE128 || MC9S12DT256 || MC9S12DP512
    // configure the uart for the requested protocol and speed
#if MCF51CN128
    if (uart == 2) {
        SCI3C1 = (loopback?SCI1C1_LOOPS_MASK:0)|((data==8&&parity!=2)?SCI1C1_M_MASK:0)|((parity!=2)?SCI1C1_PE_MASK:0)|(parity==1?SCI1C1_PT_MASK:0);
        
#if MCF51CN128
        divisor = bus_frequency*2/baud/16;
#else
        divisor = bus_frequency/baud/16;
#endif
        if (divisor >= 0x2000) {
            divisor = 0x1fff;
        }
        SCI3BDH = (uint8)(divisor/0x100);
        SCI3BDL = (uint8)(divisor%0x100);
    } else
#endif
    if (! uart) {
        SCI1C1X = (loopback?SCI1C1_LOOPS_MASK:0)|((data==8&&parity!=2)?SCI1C1_M_MASK:0)|((parity!=2)?SCI1C1_PE_MASK:0)|(parity==1?SCI1C1_PT_MASK:0);
        
#if MCF51CN128
        divisor = bus_frequency*2/baud/16;
#else
        divisor = bus_frequency/baud/16;
#endif
        if (divisor >= 0x2000) {
            divisor = 0x1fff;
        }
        SCI1BDHX = (uint8)(divisor/0x100);
        SCI1BDLX = (uint8)(divisor%0x100);
    } else {
        SCI2C1X = (loopback?SCI1C1_LOOPS_MASK:0)|((data==8&&parity!=2)?SCI1C1_M_MASK:0)|((parity!=2)?SCI1C1_PE_MASK:0)|(parity==1?SCI1C1_PT_MASK:0);
        
#if MCF51CN128
        divisor = bus_frequency*2/baud/16;
#else
        divisor = bus_frequency/baud/16;
#endif
        if (divisor >= 0x2000) {
            divisor = 0x1fff;
        }
        SCI2BDHX = (uint8)(divisor/0x100);
        SCI2BDLX = (uint8)(divisor%0x100);
    }
#elif PIC32
    if (! uart) {
        U1MODE = _U1MODE_UARTEN_MASK|(loopback?_U1MODE_LPBACK_MASK:0)|((data==8&&parity!=2)?(parity?_U1MODE_PDSEL1_MASK:_U1MODE_PDSEL0_MASK):0);

        divisor = bus_frequency/baud/16;
        if (divisor >= 0x10000) {
            divisor = 0xffff;
        }
        U1BRG = divisor;

        U1STA = _U1STA_URXEN_MASK|_U1STA_UTXEN_MASK;
    } else {
        U2MODE = _U2MODE_UARTEN_MASK|(loopback?_U2MODE_LPBACK_MASK:0)|((data==8&&parity!=2)?(parity?_U2MODE_PDSEL1_MASK:_U2MODE_PDSEL0_MASK):0);
        divisor = bus_frequency/baud/16;
        if (divisor >= 0x10000) {
            divisor = 0xffff;
        }
        U2BRG = divisor;

        U2STA = _U2STA_URXEN_MASK|_U2STA_UTXEN_MASK;
    }
#else
#error
#endif
#endif
    umask[uart] = data==8?0xff:0x7f;
}

bool
pin_uart_tx_ready(int uart)
{
#if ! STICK_GUEST
    int usr;
    
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    // if the uart transmitter is ready...
    usr = MCF_UART_USR(uart);
    if (MCF_UART_USR(uart) & MCF_UART_USR_TXRDY) {
        return true;
    }
#elif MCF51JM128 || MCF51CN128 || MCF51QE128 || MC9S08QE128 || MC9S12DT256 || MC9S12DP512
    // if the uart transmitter is ready...
#if MCF51CN128
    usr = (uart==2)?(SCI3S1):(uart?SCI2S1X:SCI1S1X);
#else
    usr = uart?SCI2S1X:SCI1S1X;
#endif
    if (usr & SCI1S1_TDRE_MASK) {
        return true;
    }
#elif PIC32
    // if the uart transmitter is ready...
    usr = uart?U2STA:U1STA;
    if (! (usr & _U1STA_UTXBF_MASK)) {
        return true;
    }
#else
#error
#endif
    return false;
#else
    return true;
#endif
}

bool
pin_uart_tx_empty(int uart)
{
#if ! STICK_GUEST
    int usr;

#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    // if the uart transmitter is empty...
    usr = MCF_UART_USR(uart);
    if (usr & MCF_UART_USR_TXEMP) {
        return true;
    }
#elif MCF51JM128 || MCF51CN128 || MCF51QE128 || MC9S08QE128 || MC9S12DT256 || MC9S12DP512
    // if the uart transmitter is empty...
#if MCF51CN128
    usr = (uart==2)?(SCI3S1):(uart?SCI2S1X:SCI1S1X);
#else
    usr = uart?SCI2S1X:SCI1S1X;
#endif
    if (usr & SCI1S1_TC_MASK) {
        return true;
    }
#elif PIC32
    // if the uart transmitter is empty...
    usr = uart?U2STA:U1STA;
    if (usr & _U1STA_TRMT_MASK) {
        return true;
    }
#else
#error
#endif
    return false;
#else
    return true;
#endif
}

bool
pin_uart_rx_ready(int uart)
{
#if ! STICK_GUEST
    int usr;

#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    // if the uart receiver is ready...
    usr = MCF_UART_USR(uart);
    if (usr & MCF_UART_USR_RXRDY) {
        return true;
    }
#elif MCF51JM128 || MCF51CN128 || MCF51QE128 || MC9S08QE128 || MC9S12DT256 || MC9S12DP512
    // if the uart receiver is ready...
#if MCF51CN128
    usr = (uart==2)?(SCI3S1):(uart?SCI2S1X:SCI1S1X);
#else
    usr = uart?SCI2S1X:SCI1S1X;
#endif
    if (usr & SCI1S1_RDRF_MASK) {
        return true;
    }
#elif PIC32
    // if the uart receiver is ready...
    usr = uart?U2STA:U1STA;
    if (usr & _U1STA_URXDA_MASK) {
        return true;
    }
#else
#error
#endif
#endif
    return false;
}

void
pin_uart_tx(int uart, byte value)
{
    int x;
    x = splx(7);
    
    while (! pin_uart_tx_ready(uart)) {
        splx(x);
        assert(gpl() < SPL_SERIAL);
        // revisit -- poll?
        x = splx(7);
    }

#if ! STICK_GUEST
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    MCF_UART_UTB(uart) = value;
#elif MCF51JM128 || MCF51CN128 || MCF51QE128 || MC9S08QE128 || MC9S12DT256 || MC9S12DP512
#if MCF51CN128
    if (uart == 2) {
        SCI3D = value;
    } else
#endif
    if (uart) {
        SCI2DX = value;
    } else {
        SCI1DX = value;
    }
#elif PIC32
    if (uart) {
        U2TXREG = value;
    } else {
        U1TXREG = value;
    }
#endif
#endif
    ulasttx[uart] = value;
    uart_armed[UART_INT(uart, true)] = true;

    splx(x);
}

byte
pin_uart_rx(int uart)
{
    int x;
    byte value;

    x = splx(7);
    
    if (pin_uart_rx_ready(uart)) {
#if ! STICK_GUEST
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
        value = MCF_UART_URB(uart);
#elif MCF51JM128 || MCF51CN128 || MCF51QE128 || MC9S08QE128 || MC9S12DT256 || MC9S12DP512
#if MCF51CN128
        if (uart == 2) {
            value = SCI3D;
        } else
#endif
        if (uart) {
            value = SCI2DX;
        } else {
            value = SCI1DX;
        }
#elif PIC32
        if (uart) {
            value = U2RXREG;
            U2STACLR = _U2STA_OERR_MASK;
        } else {
            value = U1RXREG;
            U1STACLR = _U1STA_OERR_MASK;
        }
#endif
#else
        value = 0;
#endif
        value = value & umask[uart];
    } else {
        value = 0;
    }

    uart_armed[UART_INT(uart, false)] = true;
    
    splx(x);
    
    return value;
}


void
pin_clear(void)
{
    int i;

    // N.B. we use 0 to mean no frequency type
    assert(! pin_type_digital_input);

#if ! STICK_GUEST
    i2c_uninitialize();
    qspi_uninitialize();
    // XXX -- should we unconfigure uarts?
#endif

    // reset all explicitly declared pins to digital input
    for (i = 0; i < PIN_UNASSIGNED; i++) {
        if (declared[i/8] & (1 << (i%8))) {
            if (pins[i].pin_type_mask & (1 << pin_type_digital_input)) {
                pin_declare_internal(i, pin_type_digital_input, 0, false, false);
            }
        }
    }
    
#if ! STICK_GUEST
#if MCF51JM128 || MCF51CN128 || MCF51QE128 || MC9S08QE128
    // we have to manage shared timer resources across pins
    memset(freq, pin_type_digital_input, sizeof(freq));
#elif MC9S12DT256 || MC9S12DP512
    // we have to manage a single shared frequency output timer
    memset(freq, pin_type_digital_input, sizeof(freq));
#elif PIC32
    // we have to manage shared timer resources across pins
    // REVISIT -- for now, we force timer 2 to pwm mode, disallowing two
    // different frequency output pins; we can do better in the long run
    // by dynamically allocating both timer 2 and 3 as needed.
    memset(freq, pin_type_digital_input, sizeof(freq));
#endif
#endif
}

// only called on debouncing ticks from timer_isr()
void
pin_timer_poll(void)
{
#if ! STICK_GUEST && ! FLASHER
    pin_port_sample_t *sample;

    sample = pin_digital_debounce[pin_digital_debounce_cycle];
    
    // for each port...
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    sample[port_tc] = MCF_GPIO_SETTC;
    sample[port_qs] = MCF_GPIO_SETQS;
#if MCF52259
    sample[port_ti] = MCF_GPIO_SETTI;
    sample[port_tj] = MCF_GPIO_SETTJ;
    sample[port_uc] = MCF_GPIO_SETUC;
#endif
    sample[port_ub] = MCF_GPIO_SETUB;
    sample[port_ua] = MCF_GPIO_SETUA;
    sample[port_an] = MCF_GPIO_SETAN;
    sample[port_nq] = MCF_GPIO_SETNQ;
    sample[port_as] = MCF_GPIO_SETAS;
#if MCF52233
    sample[port_gp] = MCF_GPIO_SETGP;
#endif
#if MCF52233 || MCF52259 || MCF5211
    sample[port_ta] = MCF_GPIO_SETTA;
#endif
#elif MCF51JM128 || MCF51CN128 || MCF51QE128 || MC9S08QE128
    sample[port_a] = PTAD;
    sample[port_b] = PTBD;
    sample[port_c] = PTCD;
    sample[port_d] = PTDD;
    sample[port_e] = PTED;
    sample[port_f] = PTFD;
    sample[port_g] = PTGD;
#if MCF51CN128
    sample[port_h] = PTHD;
    sample[port_j] = PTJD;
#endif
#elif MC9S12DT256 || MC9S12DP512
    sample[port_ad0] = PORTAD0;
    sample[port_ad1] = PORTAD1;
    sample[port_a] = PORTA;
    sample[port_b] = PORTB;
    sample[port_e] = PORTE;
    sample[port_j] = PTIJ;
    sample[port_m] = PTIM;
    sample[port_p] = PTIP;
    sample[port_s] = PTIS;
    sample[port_t] = PTIT;
#elif PIC32
#ifdef PIC32PORTA
    sample[port_a] = PORTA;
#endif
    sample[port_b] = PORTB;
#if PIC32PPS
#else
    sample[port_c] = PORTC;
    sample[port_d] = PORTD;
    sample[port_e] = PORTE;
    sample[port_f] = PORTF;
    sample[port_g] = PORTG;
#endif
#endif

    if (++pin_digital_debounce_cycle >= pin_digital_debounce_history_depth) {
        pin_digital_debounce_cycle = 0;
    }
#endif // ! STICK_GUEST && ! FLASHER
}

void
pin_assign(int assign, int pin)
{
    assert(assign < pin_assignment_max);
    assert(pin < PIN_LAST);
    
    pin_assignments[assign] = pin;

    if (pin < PIN_UNASSIGNED) {
        if (assign == pin_assignment_safemode
#if KBD
        || (assign >= pin_assignment_kbd_r0 && assign <= pin_assignment_kbd_r3)
#endif
       ) {
            pin_declare_internal(pin, pin_type_digital_input, 0, false, false);
        } else {
            pin_declare_internal(pin, pin_type_digital_output, 0, false, false);
        }
    }
}

extern void
pin_initialize(void)
{
#if ! STICK_GUEST
    int i;
    int32 hz;
    int32 pin;
    int32 analog;
#endif

    assert(PIN_LAST <= PIN_MAX);
    assert(pin_type_last < (sizeof(uint16)*8));
    assert(pin_qual_last < (sizeof(byte)*8));
    assert(LENGTHOF(pins) == PIN_LAST);

    memset(umask, -1, sizeof(umask));

#if ! STICK_GUEST
    // load up our servo frequency
#if STICKOS
    hz = var_get_flash(FLASH_SERVO);
#else
    hz = -1;
#endif
    if (hz != (uint32)-1 && hz >= 20 && hz <= 500) {
        servo_hz = hz;
    }

#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    // 16 bit pwm
    MCF_PWM_PWMCTL = MCF_PWM_PWMCTL_CON01|MCF_PWM_PWMCTL_CON23|MCF_PWM_PWMCTL_CON45|MCF_PWM_PWMCTL_CON67;

    // enable all pwm channels
    MCF_PWM_PWME = 0xff;
    
    // set pwm prescales to 1 for analog output or 16 for servo output
    assert(SERVO_PRESCALE == 16);
    MCF_PWM_PWMPRCLK = MCF_PWM_PWMPRCLK_PCKA(0)|MCF_PWM_PWMPRCLK_PCKB(0);
    MCF_PWM_PWMSCLA = 8;
    MCF_PWM_PWMSCLB = 8;

    // set dma timer mode registers for frequency output
    MCF_DTIM0_DTMR = MCF_DTIM_DTMR_OM|MCF_DTIM_DTMR_FRR|MCF_DTIM_DTMR_CLK_DIV1|MCF_DTIM_DTMR_RST;
    MCF_DTIM1_DTMR = MCF_DTIM_DTMR_OM|MCF_DTIM_DTMR_FRR|MCF_DTIM_DTMR_CLK_DIV1|MCF_DTIM_DTMR_RST;
    MCF_DTIM2_DTMR = MCF_DTIM_DTMR_OM|MCF_DTIM_DTMR_FRR|MCF_DTIM_DTMR_CLK_DIV1|MCF_DTIM_DTMR_RST;
    MCF_DTIM3_DTMR = MCF_DTIM_DTMR_OM|MCF_DTIM_DTMR_FRR|MCF_DTIM_DTMR_CLK_DIV1|MCF_DTIM_DTMR_RST;
#elif MCF51JM128 || MCF51CN128 || MCF51QE128 || MC9S08QE128
    // we have to manage shared timer resources across pins
    pin_clear();
    
    // enable all pullups
    PTAPE = 0xff;
    PTBPE = 0xff;
    PTCPE = 0xff;
    PTDPE = 0xff;
    PTEPE = 0xff;
    PTFPE = 0xff;
    PTGPE = 0xff;
#if MCF51CN128
    PTHPE = 0xff;
    PTJPE = 0xff;
#endif
#elif MC9S12DT256 || MC9S12DP512
    // we have to manage shared timer resources across pins
    pin_clear();
    
    // enable pullups
    PUCR = PUCR_PUPEE_MASK|PUCR_PUPBE_MASK|PUCR_PUPAE_MASK;
    PERJ = 0xff;
    PERM = 0xff;
    PERP = 0xff;
    PERS = 0xff;
    PERT = 0xff;
    
    // set pwm prescales to 8 for analog output, or 8*n for servo output
    assert(SERVO_PRESCALE == 8);
    PWMPRCLK = PWMPRCLK_PCKA1_MASK|PWMPRCLK_PCKA0_MASK|PWMPRCLK_PCKB1_MASK|PWMPRCLK_PCKB0_MASK;
    // set pwm scales for 45Hz for servo output
    assert(bus_frequency/SERVO_PRESCALE/2/256/servo_hz < 256);
    PWMSCLA = bus_frequency/SERVO_PRESCALE/2/256/servo_hz;
    PWMSCLB = bus_frequency/SERVO_PRESCALE/2/256/servo_hz;
    
    // set periods to 0xff
    PWMPER0 = 0xff;
    PWMPER1 = 0xff;
    PWMPER2 = 0xff;
    PWMPER3 = 0xff;
    PWMPER4 = 0xff;
    PWMPER5 = 0xff;
    PWMPER6 = 0xff;
    PWMPER7 = 0xff;

    // initialize timer
    TIOS = 0xff;
    
    // set timer prescale to 16
    TSCR2 = 0x0c;
    assert(FREQ_PRESCALE == 16);
#elif PIC32
    // we have to manage shared timer resources across pins
    pin_clear();

#if PIC32PPS
#else
    // enable all pullups
    CNPUE = 0x3fffff;
#endif
#else
#error
#endif

    // load up our pin assignments
    for (i = 0; i < pin_assignment_max; i++) {
#if STICKOS
        pin = var_get_flash(FLASH_ASSIGNMENTS_BEGIN+i);
#else
        pin = -1;
#endif
        if (pin != (uint32)-1 && pin >= 0 && pin < PIN_LAST) {
            pin_assignments[i] = pin;
        }
        pin_assign(i, pin_assignments[i]);
    }
    
    // if autorun disable is asserted on boot, skip autorun
    disable_autorun = ! pin_get(pin_assignments[pin_assignment_safemode], pin_type_digital_input, 0);

    // set our analog level
#if STICKOS
    analog = var_get_flash(FLASH_ANALOG);
#else
    analog = -1;
#endif
    if (analog != (uint32)-1 && analog >= 1000 && analog <= 5000) {
        pin_analog = analog;
    }
#endif
}

