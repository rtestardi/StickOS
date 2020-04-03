// *** timer.c ********************************************************
// this file implements the core interval timer used internally.

#include "main.h"
#if STICK_GUEST && __unix__
#include <signal.h>
#include <sys/time.h>
#endif

#if SODEBUG
volatile bool timer_in_isr;
volatile int32 timer_in_ticks;
volatile int32 timer_out_ticks;
volatile int32 timer_max_ticks;
#endif

volatile int32 ticks;  // incremented by pit0 isr every tick
volatile int32 msecs;  // incremented by pit0 isr every millisecond
volatile int32 seconds;  // incremented by pit0 isr every second

volatile static byte msecs_in_second;  // incremented by pit0 isr every millisecond
volatile static byte eighths_in_second;  // incremented by pit0 isr 8 times per second

enum { msecs_per_debounce = 4 }; // tunable

// called by pit0 every tick
INTERRUPT
void
#if PIC32 && ! _WIN32
__ISR(4, ipl6) // REVISIT -- ipl?
#endif
timer_isr(void)
{
    bool debouncing;
    
    assert(! timer_in_isr);
    assert((timer_in_isr = true) ? true : true);
    assert((timer_in_ticks = ticks) ? true : true);

#if ! MC9S08QE128 && ! MC9S12DT256 && ! MC9S12DP512
    (void)splx(7);
#endif

    // clear the interrupt flag
#if ! STICK_GUEST
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    MCF_PIT0_PCSR |= MCF_PIT_PCSR_PIF;
#elif MCF51JM128 || MCF51CN128 || MCF51QE128 || MC9S08QE128
    RTCSC |= RTCSC_RTIF_MASK;
#elif MC9S12DT256 || MC9S12DP512
    CRGFLG = 128;                        /* Reset interrupt request flag */
#elif PIC32
    mT1ClearIntFlag();
#else
#error
#endif
#endif // ! STICK_GUEST

    ticks++;

    // If a msec elapsed...
    if ((ticks & (ticks_per_msec - 1)) == 0) {
        msecs++;
        msecs_in_second++;

        debouncing = (msecs & (msecs_per_debounce - 1)) == 0;
    
#if ! PICTOCRYPT && ! FLASHER
        if (debouncing) {
            // poll debounced digital inputs.
            pin_timer_poll();
        }
#endif

        // if an an eighth of a second has elapsed...
        if (msecs_in_second == 125) {
            msecs_in_second = 0;
        
            eighths_in_second++;
        
            // if a second has elapsed...
            if ((eighths_in_second&7) == 0) {
                seconds++;
            }
        
            // poll the LEDs 8 times a second
            led_timer_poll();

#if PICTOCRYPT && ! MCF51JM128
            // poll for panics 8 times a second
            main_timer_poll();
#endif
        }

#if STICKOS
        // profile every msec while running
        if (running) {
            code_timer_poll();
        }
#endif

#if MCF52233 && ! STICK_GUEST
        // 200 Hz cticks
        // REVISIT -- get rid of this divide
        if (msecs%5 == 0) {
            extern volatile unsigned long cticks;
            
            cticks++;
        }
#endif

#if BADGE_BOARD && ! STICK_GUEST
        // manage LED matrix
        jm_timer_poll();
#endif
#if KBD && ! STICK_GUEST
        // manage kbd scanning every 8ms
        if ((msecs & 7) == 7) {
            kbd_timer_poll();
        }
#endif
    } else {
        debouncing = false;
    }

#if ! FLASHER
    // poll the adc to occasionally record debouncing data and to record adc data every tick.
    adc_timer_poll(debouncing);
#endif

    assert(timer_in_isr);
    assert((timer_in_isr = false) ? true : true);
    assert((timer_out_ticks = ticks) ? true : true);
    assert((timer_max_ticks = MAX(timer_max_ticks, timer_out_ticks-timer_in_ticks)) ? true : true);
}

#if STICK_GUEST
static int32 oms;  // msecs already accounted for
static int32 tbd;  // ticks to be delivered

static
int32
timer_ms(void)
{
    int32 ms;

#if _WIN32
    ms = GetTickCount();
#else
    ms = times(NULL)*10;
#endif
    ms *= isatty(0)?1:10;

    return ms;
}

// deliver timer ticks for STICK_GUEST builds
void
timer_ticks(bool align)
{
    int x;
    int32 ms;

    ms = timer_ms();

    // if we are starting a run...
    if (align) {
        // wait for a time change
        while (timer_ms() == ms) {
            // NULL
        }
        ms = timer_ms();
    }

    // if the time has changed...
    if (ms != oms) {
        // schedule the ticks for delivery
        tbd += (ms-oms)*ticks_per_msec;
        oms = ms;
    }

    // deliver the ticks
    while (tbd) {
        x = splx(7);
        timer_isr();
        splx(x);
        tbd--;

        // N.B. if we're runnning (vs. starting a run) we only deliver one tick per call
        if (! align) {
            break;
        }
    }
}
#endif

// this function initializes the timer module.
void
timer_initialize(void)
{
    assert(IS_POWER_OF_2(ticks_per_msec));
    assert(IS_POWER_OF_2(msecs_per_debounce));

#if ! STICK_GUEST
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    // enable pit0 timer interrupt
    MCF_INTC0_ICR55 = MCF_INTC_ICR_IL(SPL_PIT0)|MCF_INTC_ICR_IP(SPL_PIT0);
    MCF_INTC0_IMRL &= ~MCF_INTC_IMRL_MASKALL;
    MCF_INTC0_IMRH &= ~MCF_INTC_IMRH_INT_MASK55;  // pit0
    
    // configure pit0 to interrupt every ticks times per msec.
    MCF_PIT0_PCSR = 0;
    MCF_PIT0_PMR = bus_frequency/1000/ticks_per_msec - 1;
    MCF_PIT0_PCSR = MCF_PIT_PCSR_PRE(0)|MCF_PIT_PCSR_DOZE|MCF_PIT_PCSR_DBG|MCF_PIT_PCSR_OVW|MCF_PIT_PCSR_PIE|MCF_PIT_PCSR_RLD|MCF_PIT_PCSR_EN;
#elif MCF51JM128 || MCF51CN128 || MCF51QE128 || MC9S08QE128
#if ! MC9S08QE128 && ! MC9S12DT256 && ! MC9S12DP512
    // remap rtc to level 6
    // XXX -- this does nothing???
    INTC_PL6P7 = 27;
#endif

    // ticks_per_msec>4 requires use of IRCLK (32kHz), which is not
    // very accurate.  So, for now, limit ticks_per_msec to 4.
    assert(ticks_per_msec <= 4);

#if MCF51JM128
    // configure rtc to interrupt every ticks times per msec.
    RTCSC = RTCSC_RTCLKS0_MASK|RTCSC_RTIE_MASK|8;  // use external oscillator with prescale 1000
    RTCMOD = oscillator_frequency/1000/1000/ticks_per_msec - 1;
#else
    // run on the internal oscillator for now -- REVISIT!
    RTCSC = RTCSC_RTCLKS1_MASK|RTCSC_RTIE_MASK|8;
    RTCMOD = oscillator_frequency/1000/ticks_per_msec - 1;
#endif
#elif MC9S12DT256 || MC9S12DP512
#define setReg8(RegName, val)                                    (RegName = (byte)(val))
#define setReg16(RegName, val)                                  (RegName = (word)(val))
#define clrReg8Bits(RegName, ClrMask)                            (RegName &= ~(byte)(ClrMask))
#define setReg8Bits(RegName, SetMask)                            (RegName |= (byte)(SetMask))
#define clrSetReg8Bits(RegName, ClrMask, SetMask)                (RegName = (RegName & (~(byte)(ClrMask))) | (byte)(SetMask))
    /* Common initialization of the CPU registers */
    /* CRGINT: LOCKIE=0,SCMIE=0 */
    clrReg8Bits(CRGINT, 18);              
    /* COPCTL: RSBCK=0 */
    //clrReg8Bits(COPCTL, 64);              
    /* RDRIV: RDPK=0,RDPE=0,RDPB=0,RDPA=0 */
    clrReg8Bits(RDRIV, 147);              
    /* RDRH: RDRH7=0,RDRH6=0,RDRH5=0,RDRH4=0,RDRH3=0,RDRH2=0,RDRH1=0,RDRH0=0 */
    setReg8(RDRH, 0);                     
    /* RDRJ: RDRJ7=0,RDRJ6=0,RDRJ1=0,RDRJ0=0 */
    clrReg8Bits(RDRJ, 195);               
    /* RDRM: RDRM7=0,RDRM6=0,RDRM5=0,RDRM4=0,RDRM3=0,RDRM2=0,RDRM1=0,RDRM0=0 */
    setReg8(RDRM, 0);                     
    /* RDRP: RDRP7=0,RDRP6=0,RDRP5=0,RDRP4=0,RDRP3=0,RDRP2=0,RDRP1=0,RDRP0=0 */
    setReg8(RDRP, 0);                     
    /* RDRS: RDRS7=0,RDRS6=0,RDRS5=0,RDRS4=0,RDRS3=0,RDRS2=0,RDRS1=0,RDRS0=0 */
    setReg8(RDRS, 0);                     
    /* RDRT: RDRT7=0,RDRT6=0,RDRT5=0,RDRT4=0,RDRT3=0,RDRT2=0,RDRT1=0,RDRT0=0 */
    setReg8(RDRT, 0);                     
    /* INTCR: IRQEN=0 */
    clrReg8Bits(INTCR, 64);               
    /* ### MC9S12DT256_112 "Cpu" init code ... */
    /* ### TimerInt "TI1" init code ... */
#if MC9S12DP512
    setReg8(RTICTL, 51);  // divide by 16384
#else
    setReg8(RTICTL, 19);  // divide by 4096
#endif
    /* Common peripheral initialization - ENABLE */
    /* CRGFLG: RTIF=1 */
    setReg8Bits(CRGFLG, 128);             
    /* CRGINT: RTIE=1 */
    setReg8Bits(CRGINT, 128);             
#elif PIC32
    // configure t1 to interrupt every ticks times per msec.
    T1CON = T1_SOURCE_INT | T1_PS_1_8;
    TMR1 = 0;
    PR1 = bus_frequency/8/1000/ticks_per_msec - 1;
    T1CONSET = T1_ON;

    // set up the timer interrupt with a priority of 6
    ConfigIntTimer1(T1_INT_ON | T1_INT_PRIOR_6);
#else
#error
#endif
#else  // ! STICK_GUEST
    oms = timer_ms();
#endif // ! STICK_GUEST
}

