// *** startup.c ******************************************************
// this file is where hardware starts execution, at _startup; then we
// call init(); init() is responsible for calling main().

#include "main.h"

#if ! PIC32 && ! MC9S08QE128 && ! MC9S12DT256 && ! MC9S12DP512
extern unsigned char far _SP_INIT[], _SDA_BASE[];
extern unsigned char far __SP_AFTER_RESET[];
extern unsigned char far ___RAMBAR[], ___RAMBAR_SIZE[];
extern unsigned char far __DATA_RAM[], __DATA_ROM[], __DATA_END[];
#endif

bool disable_autorun;
uint16 flash_checksum;
bool usb_host_mode;

byte *end_of_static;

uint32 cpu_frequency;
uint32 oscillator_frequency;
uint32 bus_frequency;

bool debugger_attached;
// N.B. big_buffer may not be aligned; it is up to the caller to align it if needed.
#if PICTOCRYPT
byte big_buffer[8192];
#else
byte big_buffer[768];
#endif

#if ! PIC32 && ! MC9S08QE128 && ! MC9S12DT256 && ! MC9S12DP512

#if BADGE_BOARD || DEMO_KIT
extern void pre_main(void);
#endif
extern int main();

// this function performs C initialization before main() runs.
#if ! GCC
static
DECLSPEC_STARTUP
#endif
void
init(void)
{
    byte *p;
    uint32 n;
    uint8 *dp, *sp;

    // zero all of RAM
    memset((void *)__DATA_RAM, 0, (uint32)_SP_INIT - (uint32)__DATA_RAM - 64);

    // move initialized data from ROM to RAM.
    if (__DATA_ROM != __DATA_RAM) {
        dp = (uint8 *)__DATA_RAM;
        sp = (uint8 *)__DATA_ROM;
        n = __DATA_END - __DATA_RAM;
        while (n--) {
            *dp++ = *sp++;
        }
    }

    // disable Software Watchdog Timer
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
#if ! MCF5211
    // enable debug
    MCF_GPIO_PDDPAR = 0x0F;
#endif
    MCF_SCM_CWCR = 0;
#elif MCF51JM128
    SOPT1 = SOPT1_STOPE_MASK;
    SOPT2 &= ~SOPT2_USB_BIGEND_MASK;
#elif MCF51CN128
    SOPT1 = SOPT1_STOPE_MASK;
#elif MCF51QE128
    SOPT1 = SOPT1_STOPE_MASK|SOPT1_BKGDPE_MASK;
#else
#error
#endif

    // set the clocks
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
#if MCF5211
    // we use the 8MHz internal oscillator divided by 1
    // and multiply by 8 to get 64MHz
    MCF_CLOCK_SYNCR = MCF_CLOCK_SYNCR_MFD(2)|MCF_CLOCK_SYNCR_CLKSRC|MCF_CLOCK_SYNCR_PLLMODE|MCF_CLOCK_SYNCR_PLLEN;
    
    // no USB
    cpu_frequency = 64000000;
    bus_frequency = cpu_frequency/2;
    oscillator_frequency = 8000000;
#elif MCF52221
    // if we don't have a crystal...
    if (MCF_CLOCK_SYNSR & MCF_CLOCK_SYNSR_OCOSC) {
        // we use the 8MHz internal oscillator divided by 1
        MCF_CLOCK_CCHR = 0;

        // and multiply by 6 to get 48MHz
        MCF_CLOCK_SYNCR = MCF_CLOCK_SYNCR_MFD(1)|MCF_CLOCK_SYNCR_CLKSRC|MCF_CLOCK_SYNCR_PLLMODE|MCF_CLOCK_SYNCR_PLLEN;
        
        // USB uses fsys
        cpu_frequency = 48000000;
        bus_frequency = cpu_frequency/2;
        oscillator_frequency = 8000000;
    } else {
        // we use the 48MHz crystal divided by 6
        MCF_CLOCK_CCHR = 5;

        // and multiply by 8 to get 64MHz
        MCF_CLOCK_SYNCR = MCF_CLOCK_SYNCR_MFD(2)|MCF_CLOCK_SYNCR_CLKSRC|MCF_CLOCK_SYNCR_PLLMODE|MCF_CLOCK_SYNCR_PLLEN;
        
        // USB uses oscillator
        cpu_frequency = 64000000;
        bus_frequency = cpu_frequency/2;
        oscillator_frequency = 48000000;
    }
#elif MCF52233
    // we use the 25MHz crystal divided by 5
    MCF_CLOCK_CCHR = 4;

    // and multiply by 12 to get 60MHz
    MCF_CLOCK_SYNCR = MCF_CLOCK_SYNCR_MFD(4)|MCF_CLOCK_SYNCR_CLKSRC|MCF_CLOCK_SYNCR_PLLMODE|MCF_CLOCK_SYNCR_PLLEN;
    
    // no USB
    cpu_frequency = 60000000;
    bus_frequency = cpu_frequency/2;
    oscillator_frequency = 25000000;
#elif MCF52259
    MCF_CLOCK_OCLR = 0xC0;  //turn on crystal
    MCF_CLOCK_CCLR = 0x00;  //switch to crystal 
    MCF_CLOCK_OCHR = 0x00;  //turn off relaxation osc

    // we use the 48MHz crystal divided by 6
    MCF_CLOCK_CCHR = 5;
    
    // and multiply by 10 to get 80MHz
    MCF_CLOCK_SYNCR = MCF_CLOCK_SYNCR_MFD(3)|MCF_CLOCK_SYNCR_CLKSRC|MCF_CLOCK_SYNCR_PLLMODE|MCF_CLOCK_SYNCR_PLLEN;

    // USB uses oscillator
    cpu_frequency = 80000000;
    bus_frequency = cpu_frequency/2;
    oscillator_frequency = 48000000;
    
    // shut down non-essential clocks
    MCF_SCM_PPMRL = MCF_SCM_PPMRL_CDFEC|MCF_SCM_PPMRL_CDMINIBUS;
    MCF_SCM_PPMRH = MCF_SCM_PPMRH_CDPIT1;
#else
#error
#endif

    // wait for pll lock
    while (!(MCF_CLOCK_SYNSR & MCF_CLOCK_SYNSR_LOCK)) {
        // NULL
    }
    
    // set real time clock freq
#if MCF52259
    MCF_RTC_RTCGOCU = 0x8000>>16;
    MCF_RTC_RTCGOCL = 0x8000;
#elif ! MCF5211
    MCF_CLOCK_RTCDR = cpu_frequency;
#endif

    // enable on-chip modules to access internal SRAM
    MCF_SCM_RAMBAR = (0|MCF_SCM_RAMBAR_BA(RAMBAR_ADDRESS)|MCF_SCM_RAMBAR_BDE);
        
    // we read MCF_GPIO_PORTAN's initial value to determine if the debugger is
    // attached
    // N.B. this value must be set by the debugger's cfg file!
    if (! MCF_GPIO_PORTAN) {
        debugger_attached = true;
    } else {
        // turn off pstclk to reduce emi
        MCF_CLOCK_SYNCR |= MCF_CLOCK_SYNCR_DISCLK;
    }
#elif MCF51JM128
    /* switch from FEI to FBE (FLL bypassed external) */ 
    /* enable external clock source */
    MCGC2 = MCGC2_HGO_MASK       /* oscillator in high gain mode */
          | MCGC2_EREFS_MASK   /* because crystal is being used */
          | MCGC2_RANGE_MASK   /* 12 MHz is in high freq range */
          | MCGC2_ERCLKEN_MASK;     /* activate external reference clock */
    while (MCGSC_OSCINIT == 0)
    ;
    /* select clock mode */
    MCGC1 = (2<<6)         /* CLKS = 10 -> external reference clock. */
        | (3<<3);          /* RDIV = 3 -> 12MHz/8=1.5 MHz */

    /* wait for mode change to be done */
    while (MCGSC_IREFST != 0)
    ;
    while (MCGSC_CLKST != 2)
    ;

    /* switch from FBE to PBE (PLL bypassed internal) mode */
    MCGC3=MCGC3_PLLS_MASK
        | (8<<0);     /* VDIV=6 -> multiply by 32 -> 1.5MHz * 32 = 48MHz */
    while(MCGSC_PLLST != 1)
    ;
    while(MCGSC_LOCK != 1)
    ;
    /* finally switch from PBE to PEE (PLL enabled external mode) */
    MCGC1 = (0<<6)         /* CLKS = 0 -> PLL or FLL output clock. */
          | (3<<3);        /* RDIV = 3 -> 12MHz/8=1.5 MHz */
    while(MCGSC_CLKST!=3)
    ;

    /* Now MCGOUT=48MHz, BUS_CLOCK=24MHz */  
    cpu_frequency = 48000000;
    bus_frequency = cpu_frequency/2;
    oscillator_frequency = 12000000;

    // we read KBI1SC's initial value to determine if the debugger is attached
    // N.B. this value must be set by the debugger's cmd file!
    if (KBI1SC == 0x01) {
        debugger_attached = true;
    }
#elif MCF51CN128
#define EXTAL_PIN_INIT                  PTDPF1_D4 = 0b11;   // extal pin ON
#define XTAL_PIN_INIT                   PTDPF1_D5 = 0b11;   // xtal pin ON
    EXTAL_PIN_INIT;
    XTAL_PIN_INIT;

  /* switch from FEI to FBE (FLL bypassed external) */ 
  /* enable external clock source */

    MCGC2 = MCGC2_ERCLKEN_MASK  // activate external reference clock
          | MCGC2_EREFS_MASK    // because crystal is being used
          | MCGC2_RANGE_MASK;   // high range  
            
    /* select clock mode */
    MCGC1 = (0b10<<6)           // CLKS = 10 -> external reference clock
          | (0b100<<3)          // RDIV = 2^4 -> 25MHz/16 = 1.5625 MHz
          | MCGC1_IRCLKEN_MASK; // IRCLK to RTC enabled
          // also clear IREFs
      
    /* wait for mode change to be done */
    while (MCGSC_IREFST | (MCGSC_CLKST != 0b10)) // wait for Reference Status bit to update
    ;                                            // and for clock status bits to update 

    /* switch from FBE to PBE (PLL bypassed internal) mode */
    MCGC3 =  (0b1000<<0)        // set PLL multi 50MHz
          |  MCGC3_PLLS_MASK;   // select PLL

    while (!MCGSC_PLLST | !MCGSC_LOCK) /* wait for PLL status bit to update */
    ;   /* Wait for LOCK bit to set */

    /* Now running PBE Mode */
    /* finally switch from PBE to PEE (PLL enabled external mode) */
    MCGC1_CLKS  = 0b00; // PLL clock to system (MCGOUT)

    /* Wait for clock status bits to update */
    while (MCGSC_CLKST != 0b11)           
    ;

    cpu_frequency = 50000000;
    bus_frequency = cpu_frequency/2;
    oscillator_frequency = 32768;
    
    // we read KBI1SC's initial value to determine if the debugger is attached
    // N.B. this value must be set by the debugger's cmd file!
    if (KBI1SC == 0x01) {
        debugger_attached = true;
    }
#elif MCF51QE128
#define setReg8(RegName, val)  (RegName = (byte)(val))
#define clrSetReg8Bits(RegName, ClrMask, SetMask)  (RegName = (byte)((byte)((byte)RegName & (~(byte)(ClrMask))) | (byte)(SetMask)))

    /*  System clock initialization */
    /* ICSC1: CLKS=0,RDIV=0,IREFS=1,IRCLKEN=1,IREFSTEN=0 */
    setReg8(ICSC1, 0x06);                /* Initialization of the ICS control register 1 */ 
    /* ICSC2: BDIV=0,RANGE=0,HGO=0,LP=0,EREFS=0,ERCLKEN=0,EREFSTEN=0 */
    setReg8(ICSC2, 0x00);                /* Initialization of the ICS control register 2 */ 
    while(!ICSSC_IREFST) {               /* Wait until the source of reference clock is internal clock */
    }
    /* ICSSC: DRST_DRS=2,DMX32=0 */
    clrSetReg8Bits(ICSSC, 0x60, 0x80);   /* Initialization of the ICS status and control */ 
    while((ICSSC & 0xC0) != 0x80) {      /* Wait until the FLL switches to High range DCO mode */
    }
    
    cpu_frequency = 50330000;
    bus_frequency = cpu_frequency/2;
    oscillator_frequency = 32768;
    
    // we read KBI1SC's initial value to determine if the debugger is attached
    // N.B. this value must be set by the debugger's cmd file!
    if (KBI1SC == 0x01) {
        debugger_attached = true;
    }
#else
#error
#endif

#if MCF51JM128
    PTGPE = 0x01;
#elif MCF51CN128
    PTGPE = 0x40;
#elif MCF51QE128
    PTAPE = 0x04;
#endif

    // flash beyond this point is available for runtime data
    end_of_static = __DATA_ROM + (__DATA_END - __DATA_RAM);

    // compute flash checksum
    for (p = (byte *)0; p < end_of_static; p++) {
        flash_checksum += *p;
    }

#if BADGE_BOARD || DEMO_KIT
    pre_main();
#endif

#if ! FLASHER && ! PICTOCRYPT
    // initialize pins
    pin_initialize();
#endif

    // finally, call main()
    main();
}

#if ! GCC
extern
BEGIN_NAKED(_startup);
#endif

// this function performs assembly language initialization from
// reset, and then calls C initialization, which calls main().
#if MCF51JM128 || MCF51QE128 || MCF51CN128 || MCF52221
DECLSPEC_STARTUP
#endif
BEGIN_NAKED(_startup)
{
    // disable interrupts
    Q3(move.w,  #0x2700,sr)

#if ! MCF51JM128 && ! MCF51CN128 && ! MCF51QE128
    // initialize RAMBAR
    Q3(move.l,  #__RAMBAR+0x21,d0)
    Q3(movec,   d0,RAMBAR)

    // initialize IPSBAR
    Q3(move.l,  #__IPSBAR,d0)
    Q3(andi.l,  #0xC0000000,d0)  // need to mask
    Q3(add.l,   #0x1,d0)
    Q3(move.l,  d0,0x40000000)

    // initialize FLASHBAR
    Q3(move.l,  #__FLASHBAR,d0)
    Q3(andi.l,  #0xFFF80000,d0)  // need to mask
#if MCF52259
    Q3(add.l,   #0x21,d0)
#else
    Q3(add.l,   #0x61,d0)
#endif
    Q3(movec,   d0,FLASHBAR)
#else
    Q3(move.l,  #0xc0000000,d0)
    Q3(movec,   d0,CPUCR)
#endif

    // set up the real stack pointer
    Q3(lea,     _SP_INIT,a7)

    // set up short data base A5
    Q3(lea,     _SDA_BASE,a5)

    // set up A6 dummy stackframe
    Q3(movea.l, #0,a6)
    Q3(link,    a6,#0)

    // C initialization, links to main
    Q2(jsr,     init)

    Q1(nop)
    Q1(halt)
}
END_NAKED

#if ! GCC
BEGIN_NAKED(asm_xxx);
#endif

#if MCF51JM128 || MCF51QE128 || MCF51CN128 || MCF52221
DECLSPEC_STARTUP
#endif
BEGIN_NAKED(asm_xxx)
{
    Q1(halt)
    Q1(rte)
}
END_NAKED

#endif // ! PIC32 && ! MC9S08QE128 && ! MC9S12DT256 && ! MC9S12DP512

