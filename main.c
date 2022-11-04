// *** main.c *********************************************************
// this is the main program that is launched by startup.c; it just
// initializes all of the modules of the os and then runs the main
// application program loop.

#include "main.h"

#if PIC32 && ! HIDBL && ! CHIPKIT
#if defined(USBOTG)
    #pragma config UPLLEN   = ON            // USB PLL Enabled
#endif
    #pragma config FPLLMUL  = MUL_20        // PLL Multiplier
#if defined(USBOTG)
    #pragma config UPLLIDIV = DIV_2         // USB PLL Input Divider
#endif
    #pragma config FPLLIDIV = DIV_2         // PLL Input Divider
#if defined(__32MX250F128B__)
    #pragma config FPLLODIV = DIV_2         // PLL Output Divider
#else
    #pragma config FPLLODIV = DIV_1         // PLL Output Divider
#endif
    #pragma config FPBDIV   = DIV_1         // Peripheral Clock divisor
    #pragma config FWDTEN   = OFF           // Watchdog Timer 
    #pragma config WDTPS    = PS1           // Watchdog Timer Postscale
    #pragma config FCKSM    = CSDCMD        // Clock Switching & Fail Safe Clock Monitor
    #pragma config OSCIOFNC = OFF           // CLKO Enable
    #pragma config POSCMOD  = XT            // Primary Oscillator
    #pragma config IESO     = OFF           // Internal/External Switch-over
    #pragma config FSOSCEN  = OFF           // Secondary Oscillator Enable
#if 0 && defined(__32MX250F128B__)
    #pragma config FNOSC    = FRCPLL        // Oscillator Selection
#else
    #pragma config FNOSC    = PRIPLL        // Oscillator Selection
#endif
    #pragma config CP       = OFF           // Code Protect
    #pragma config BWP      = OFF           // Boot Flash Write Protect
    #pragma config PWP      = OFF           // Program Flash Write Protect
#if defined(__32MX250F128B__)
    #pragma config ICESEL   = ICS_PGx1      // ICE/ICD Comm Channel Select
#else
    #pragma config ICESEL   = ICS_PGx2      // ICE/ICD Comm Channel Select
#endif
    #pragma config DEBUG    = OFF           // Debugger Disabled for Starter Kit
#endif

#if PIC32
void _general_exception_context(void)
{
    led_hex(_CP0_GET_CAUSE());
}

void _bootstrap_exception_handler(void)
{
    led_hex(_CP0_GET_CAUSE());
}
#endif

int
#if MCF52233
main2()  // the tasking system is called by startup.c, before us
#else
main()  // we're called directly by startup.c
#endif
{
    int x;

#if PIC32
    byte *p;
    //extern unsigned char _data_image_begin[];

#if defined(__32MX250F128B__)
    SYSTEMConfigPerformance(40000000L);
#else
    SYSTEMConfigPerformance(80000000L);
#endif
    INTEnableSystemMultiVectoredInt();
    (void)splx(7);
#if ! DEBUGGING
    DDPCON = 0;  // disable JTAG
#else
    debugger_attached = true;
#endif

    // slow us down
    // N.B. we can't rely on config bits since the bootloader sets them differently
    SYSKEY = 0xAA996655; // Write Key1 to SYSKEY
    SYSKEY = 0x556699AA; // Write Key2 to SYSKEY
#if defined(__32MX250F128B__)
    OSCCONbits.PBDIV = 0;
#else
    OSCCONbits.PBDIV = 1;
#endif
    SYSKEY = 0;

#if defined(__32MX250F128B__)
    cpu_frequency = 40000000;
    oscillator_frequency = 8000000;
    bus_frequency = 40000000;
#else
    cpu_frequency = 80000000;
    oscillator_frequency = 8000000;
    bus_frequency = 40000000;
#endif

    //end_of_static = _data_image_begin;

    // if rd6 is asserted on boot, skip autorun
    pin_initialize();

    // compute flash checksum
    for (p = (byte *)FLASH_START; p < end_of_static; p++) {
        flash_checksum += *p;
    }
#elif MC9S08QE128
    /* Common initialization of the write once registers */
    /* SOPT1: COPE=0,COPT=1,STOPE=0,RSTOPE=0,BKGDPE=1,RSTPE=0 */
    SOPT1 = 0x42;                                      
    /* SPMSC1: LVDF=0,LVDACK=0,LVDIE=0,LVDRE=1,LVDSE=1,LVDE=1,BGBE=0 */
    SPMSC1 = 0x1C;                                      
    /* SPMSC2: LPR=0,LPRS=0,LPWUI=0,PPDF=0,PPDACK=0,PPDE=1,PPDC=0 */
    SPMSC2 = 0x02;                                      
    /* SPMSC3: LVDV=0,LVWV=0,LVWIE=0 */
    SPMSC3 &= (unsigned char)~0x38;                     
    
    /*  System clock initialization */
    /* ICSC1: CLKS=0,RDIV=0,IREFS=1,IRCLKEN=1,IREFSTEN=0 */
    ICSC1 = 0x06;                        /* Initialization of the ICS control register 1 */
    /* ICSC2: BDIV=0,RANGE=0,HGO=0,LP=0,EREFS=0,ERCLKEN=0,EREFSTEN=0 */
    ICSC2 = 0x00;                        /* Initialization of the ICS control register 2 */
    while(!ICSSC_IREFST) {               /* Wait until the source of reference clock is internal clock */
    }
    /* ICSSC: DRST_DRS=2,DMX32=0 */
    ICSSC = (ICSSC & (unsigned char)~0x60) | (unsigned char)0x80; /* Initialization of the ICS status and control */
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

    // if pta2 is asserted on boot, skip autorun
    pin_initialize();
#elif MC9S12DT256 || MC9S12DP512
#define setReg8(RegName, val)                                    (RegName = (byte)(val))
#define setReg8Bits(RegName, SetMask)                            (RegName |= (byte)(SetMask))
#define INITRG_ADR  0x0011             /* Register map position register */
    /* ### MC9S12DT256_112 "Cpu" init code ... */
    /*  PE initialization code after reset */
    /* Initialization of the registers INITRG, INITRM, INITEE is done to protect them to be written accidentally later by the application */
    *(byte*)INITRG_ADR = 0;              /* Set the register map position */
    asm("nop");                          /* nop instruction */
    /* INITRM: RAM15=0,RAM14=0,RAM13=0,RAM12=0,RAM11=1,??=0,??=0,RAMHAL=1 */
    setReg8(INITRM, 9);                  /* Set the RAM map position */ 
    /* INITEE: EE15=0,EE14=0,EE13=0,EE12=0,??=0,??=0,??=0,EEON=1 */
    setReg8(INITEE, 1);                  /* Set the EEPROM map position */ 
    /* MISC: ??=0,??=0,??=0,??=0,EXSTR1=1,EXSTR0=1,ROMHM=0,ROMON=1 */
    setReg8(MISC, 13);                    
    /* PEAR: NOACCE=0,??=0,PIPOE=0,NECLK=0,LSTRE=0,RDWE=0,??=0,??=0 */
    setReg8(PEAR, 0);                     
    /*  System clock initialization */
    /* CLKSEL: PLLSEL=0,PSTP=0,SYSWAI=0,ROAWAI=0,PLLWAI=0,CWAI=0,RTIWAI=0,COPWAI=0 */
    setReg8(CLKSEL, 0);                  /* Select clock source from XTAL and set bits in CLKSEL reg. */ 
    /* PLLCTL: CME=1,PLLON=0,AUTO=1,ACQ=1,??=0,PRE=0,PCE=0,SCME=1 */
    setReg8(PLLCTL, 177);                /* Disable the PLL */ 
    /* SYNR: ??=0,??=0,SYN5=0,SYN4=1,SYN3=1,SYN2=0,SYN1=0,SYN0=0 */
    setReg8(SYNR, 24);                   /* Set the multiplier register */ 
#if MC9S12DT256
    /* REFDV: ??=0,??=0,??=0,??=0,REFDV3=0,REFDV2=0,REFDV1=1,REFDV0=1 */
    setReg8(REFDV, 3);                   /* Set the divider register */ 
#else
    /* REFDV: ??=0,??=0,??=0,??=0,REFDV3=1,REFDV2=1,REFDV1=1,REFDV0=1 */
    setReg8(REFDV, 15);                  /* Set the divider register */ 
#endif
    /* PLLCTL: CME=1,PLLON=1,AUTO=1,ACQ=1,??=0,PRE=0,PCE=0,SCME=1 */
    setReg8(PLLCTL, 241);                 
    while(!CRGFLG_LOCK) {                /* Wait until the PLL is within the desired tolerance of the target frequency */
    }
    /* CLKSEL: PLLSEL=1 */
    setReg8Bits(CLKSEL, 128);            /* Select clock source from PLL */ 
    /*** End of PE initialization code after reset ***/

    cpu_frequency = 50000000;
    bus_frequency = cpu_frequency/2;
#if MC9S12DT256
    oscillator_frequency = 4000000;
#else
    oscillator_frequency = 16000000;
#endif

    // we read RDRH's initial value to determine if the debugger is attached
    // N.B. this value must be set by the debugger's cmd file!
    if (RDRH == 0x01) {
        debugger_attached = true;
    }

    // if pp0 is asserted on boot, skip autorun
    pin_initialize();
#endif

    assert(sizeof(byte) == 1);
    assert(sizeof(uint16) == 2);
    assert(sizeof(uint32) == 4);

    // configure leds
    led_initialize();

    // initialize timer
    timer_initialize();
    
    // calibrate our busywait
    x = splx(0);
    delay(20);
    splx(x);
    
#if ! FLASHER
    // initialize adc
    adc_initialize();
#endif
    
#if PICTOCRYPT
    adc_timer_poll(false);

    // determine if we're in host or device mode
    if (adc_result[0] < 10000) {
        usb_host_mode = false;
    } else {
        usb_host_mode = true;
    }
#elif USB_HOST
    usb_host_mode = (var_get_flash(FLASH_USBHOST) == 1) && ! disable_autorun;
#endif

#if PICTOCRYPT
    // initialize sleep
    sleep_initialize();
#endif

#if ! FLASHER
    // initialize flash
    flash_initialize();

#if ! PICTOCRYPT && ! BADGE_BOARD
    serial_initialize();
#endif

#if USBOTG
    // initialize usb
    usb_initialize();
#endif
#endif

    // initialize the application
    main_initialize();

#if ! MCF52233
    // enable interrupts
    splx(0);
#endif

#if ! FLASHER && ! PICTOCRYPT
    // initialize zigflea
    zb_initialize();
    
    // initialize the terminal interface
    terminal_initialize();
#endif

#if BADGE_BOARD
    // initialize badge board
    jm_initialize();
#endif

    // run the main application program loop
    main_run();

    ASSERT(0);  // stop!
    return 0;
}

#if MC9S08QE128
#pragma CODE_SEG __NEAR_SEG NON_BANKED
/*
** ===================================================================
**     Interrupt handler : isrVrtc
**
**     Description :
**         User interrupt service routine. 
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
__interrupt void isrVrtc(void)
{
  /* Write your interrupt code here ... */
  timer_isr();
}
/* end of isrVrtc */


/*
** ===================================================================
**     Interrupt handler : isrVsci1rx
**
**     Description :
**         User interrupt service routine. 
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
__interrupt void isrVsci1rx(void)
{
  /* Write your interrupt code here ... */
  serial_isr();
}
/* end of isrVsci1rx */


/*
** ===================================================================
**     Interrupt handler : isrVlvd
**
**     Description :
**         User interrupt service routine. 
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
__interrupt void isrVlvd(void)
{
  /* Write your interrupt code here ... */
  ASSERT(0);
}
/* end of isrVlvd */


/*
** ===================================================================
**     Interrupt handler : isrVirq
**
**     Description :
**         User interrupt service routine. 
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
__interrupt void isrVirq(void)
{
  /* Write your interrupt code here ... */
  zb_isr();
}
/* end of isrVirq */



#pragma CODE_SEG DEFAULT
/* Initialization of the CPU registers in FLASH */

/* NVPROT: FPS6=1,FPS5=1,FPS4=1,FPS3=1,FPS2=1,FPS1=1,FPS0=1,FPOPEN=1 */
const unsigned char NVPROT_INIT @0x0000FFBD = 0xFF;

/* NVOPT: KEYEN1=0,KEYEN0=1,SEC1=1,SEC0=0 */
const unsigned char NVOPT_INIT @0x0000FFBF = 0x7E;



extern near void _Startup(void);

/* Interrupt vector table */
#ifndef UNASSIGNED_ISR
  #define UNASSIGNED_ISR 0xFFFF        /* unassigned interrupt service routine */
#endif

void (* near const _vect[])() @0xFFC0 = { /* Interrupt vector table */
         UNASSIGNED_ISR,               /* Int.no.  0 Vtpm3ovf (at FFC0)              Unassigned */
         UNASSIGNED_ISR,               /* Int.no.  1 Vtpm3ch5 (at FFC2)              Unassigned */
         UNASSIGNED_ISR,               /* Int.no.  2 Vtpm3ch4 (at FFC4)              Unassigned */
         UNASSIGNED_ISR,               /* Int.no.  3 Vtpm3ch3 (at FFC6)              Unassigned */
         UNASSIGNED_ISR,               /* Int.no.  4 Vtpm3ch2 (at FFC8)              Unassigned */
         UNASSIGNED_ISR,               /* Int.no.  5 Vtpm3ch1 (at FFCA)              Unassigned */
         UNASSIGNED_ISR,               /* Int.no.  6 Vtpm3ch0 (at FFCC)              Unassigned */
         isrVrtc,                      /* Int.no.  7 Vrtc (at FFCE)                  Used */
         UNASSIGNED_ISR,               /* Int.no.  8 Vsci2tx (at FFD0)               Unassigned */
         UNASSIGNED_ISR,               /* Int.no.  9 Vsci2rx (at FFD2)               Unassigned */
         UNASSIGNED_ISR,               /* Int.no. 10 Vsci2err (at FFD4)              Unassigned */
         UNASSIGNED_ISR,               /* Int.no. 11 Vacmpx (at FFD6)                Unassigned */
         UNASSIGNED_ISR,               /* Int.no. 12 Vadc (at FFD8)                  Unassigned */
         UNASSIGNED_ISR,               /* Int.no. 13 Vkeyboard (at FFDA)             Unassigned */
         UNASSIGNED_ISR,               /* Int.no. 14 Viicx (at FFDC)                 Unassigned */
         UNASSIGNED_ISR,               /* Int.no. 15 Vsci1tx (at FFDE)               Unassigned */
         isrVsci1rx,                   /* Int.no. 16 Vsci1rx (at FFE0)               Used */
         UNASSIGNED_ISR,               /* Int.no. 17 Vsci1err (at FFE2)              Unassigned */
         UNASSIGNED_ISR,               /* Int.no. 18 Vspi1 (at FFE4)                 Unassigned */
         UNASSIGNED_ISR,               /* Int.no. 19 Vspi2 (at FFE6)                 Unassigned */
         UNASSIGNED_ISR,               /* Int.no. 20 Vtpm2ovf (at FFE8)              Unassigned */
         UNASSIGNED_ISR,               /* Int.no. 21 Vtpm2ch2 (at FFEA)              Unassigned */
         UNASSIGNED_ISR,               /* Int.no. 22 Vtpm2ch1 (at FFEC)              Unassigned */
         UNASSIGNED_ISR,               /* Int.no. 23 Vtpm2ch0 (at FFEE)              Unassigned */
         UNASSIGNED_ISR,               /* Int.no. 24 Vtpm1ovf (at FFF0)              Unassigned */
         UNASSIGNED_ISR,               /* Int.no. 25 Vtpm1ch2 (at FFF2)              Unassigned */
         UNASSIGNED_ISR,               /* Int.no. 26 Vtpm1ch1 (at FFF4)              Unassigned */
         UNASSIGNED_ISR,               /* Int.no. 27 Vtpm1ch0 (at FFF6)              Unassigned */
         isrVlvd,                      /* Int.no. 28 Vlvd (at FFF8)                  Used */
         isrVirq,                      /* Int.no. 29 Virq (at FFFA)                  Used */
         UNASSIGNED_ISR,               /* Int.no. 30 Vswi (at FFFC)                  Unassigned */
         _Startup                      /* Int.no. 31 Vreset (at FFFE)                Reset vector */
};

#elif MC9S12DT256 || MC9S12DP512

#pragma CODE_SEG __NEAR_SEG NON_BANKED
__interrupt void AS1_Interrupt(void)
{
    serial_isr();
}

#pragma CODE_SEG __NEAR_SEG NON_BANKED
__interrupt void TI1_Interrupt(void)
{
    timer_isr();
}

#pragma CODE_SEG __NEAR_SEG NON_BANKED
__interrupt void IRQ_Interrupt(void)
{
    zb_isr();
}

#pragma CODE_SEG __NEAR_SEG NON_BANKED
__interrupt void Cpu_Interrupt(void)
{
    asm_halt();
}

extern near void _Startup(void);

/* ISR prototype */
typedef void (*near tIsrFunc)(void);
const tIsrFunc _InterruptVectorTable[] @0xFF80 = { /* Interrupt vector table */
  /* ISR name                               No. Address Pri Name          Description */
  Cpu_Interrupt,                       /* 0x3F  0xFF80   1  ivVReserved63 unused by PE */
  Cpu_Interrupt,                       /* 0x3E  0xFF82   1  ivVReserved62 unused by PE */
  Cpu_Interrupt,                       /* 0x3D  0xFF84   1  ivVReserved61 unused by PE */
  Cpu_Interrupt,                       /* 0x3C  0xFF86   1  ivVReserved60 unused by PE */
  Cpu_Interrupt,                       /* 0x3B  0xFF88   1  ivVReserved59 unused by PE */
  Cpu_Interrupt,                       /* 0x3A  0xFF8A   1  ivVReserved58 unused by PE */
  Cpu_Interrupt,                       /* 0x39  0xFF8C   1  ivVpwmesdn    unused by PE */
  Cpu_Interrupt,                       /* 0x38  0xFF8E   1  ivVportp      unused by PE */
  Cpu_Interrupt,                       /* 0x37  0xFF90   1  ivVcan4tx     unused by PE */
  Cpu_Interrupt,                       /* 0x36  0xFF92   1  ivVcan4rx     unused by PE */
  Cpu_Interrupt,                       /* 0x35  0xFF94   1  ivVcan4err    unused by PE */
  Cpu_Interrupt,                       /* 0x34  0xFF96   1  ivVcan4wkup   unused by PE */
  Cpu_Interrupt,                       /* 0x33  0xFF98   1  ivVReserved51 unused by PE */
  Cpu_Interrupt,                       /* 0x32  0xFF9A   1  ivVReserved50 unused by PE */
  Cpu_Interrupt,                       /* 0x31  0xFF9C   1  ivVReserved49 unused by PE */
  Cpu_Interrupt,                       /* 0x30  0xFF9E   1  ivVReserved48 unused by PE */
  Cpu_Interrupt,                       /* 0x2F  0xFFA0   1  ivVReserved47 unused by PE */
  Cpu_Interrupt,                       /* 0x2E  0xFFA2   1  ivVReserved46 unused by PE */
  Cpu_Interrupt,                       /* 0x2D  0xFFA4   1  ivVReserved45 unused by PE */
  Cpu_Interrupt,                       /* 0x2C  0xFFA6   1  ivVReserved44 unused by PE */
  Cpu_Interrupt,                       /* 0x2B  0xFFA8   1  ivVcan1tx     unused by PE */
  Cpu_Interrupt,                       /* 0x2A  0xFFAA   1  ivVcan1rx     unused by PE */
  Cpu_Interrupt,                       /* 0x29  0xFFAC   1  ivVcan1err    unused by PE */
  Cpu_Interrupt,                       /* 0x28  0xFFAE   1  ivVcan1wkup   unused by PE */
  Cpu_Interrupt,                       /* 0x27  0xFFB0   1  ivVcan0tx     unused by PE */
  Cpu_Interrupt,                       /* 0x26  0xFFB2   1  ivVcan0rx     unused by PE */
  Cpu_Interrupt,                       /* 0x25  0xFFB4   1  ivVcan0err    unused by PE */
  Cpu_Interrupt,                       /* 0x24  0xFFB6   1  ivVcan0wkup   unused by PE */
  Cpu_Interrupt,                       /* 0x23  0xFFB8   1  ivVflash      unused by PE */
  Cpu_Interrupt,                       /* 0x22  0xFFBA   1  ivVeeprom     unused by PE */
  Cpu_Interrupt,                       /* 0x21  0xFFBC   1  ivVspi2       unused by PE */
  Cpu_Interrupt,                       /* 0x20  0xFFBE   1  ivVspi1       unused by PE */
  Cpu_Interrupt,                       /* 0x1F  0xFFC0   1  ivViic        unused by PE */
  Cpu_Interrupt,                       /* 0x1E  0xFFC2   1  ivVReserved30 unused by PE */
  Cpu_Interrupt,                       /* 0x1D  0xFFC4   1  ivVcrgscm     unused by PE */
  Cpu_Interrupt,                       /* 0x1C  0xFFC6   1  ivVcrgplllck  unused by PE */
  Cpu_Interrupt,                       /* 0x1B  0xFFC8   1  ivVtimpabovf  unused by PE */
  Cpu_Interrupt,                       /* 0x1A  0xFFCA   1  ivVtimmdcu    unused by PE */
  Cpu_Interrupt,                       /* 0x19  0xFFCC   1  ivVporth      unused by PE */
  Cpu_Interrupt,                       /* 0x18  0xFFCE   1  ivVportj      unused by PE */
  Cpu_Interrupt,                       /* 0x17  0xFFD0   1  ivVatd1       unused by PE */
  Cpu_Interrupt,                       /* 0x16  0xFFD2   1  ivVatd0       unused by PE */
  Cpu_Interrupt,                       /* 0x15  0xFFD4   1  ivVsci1       unused by PE */
  AS1_Interrupt,                       /* 0x14  0xFFD6   1  ivVsci0       used by PE */
  Cpu_Interrupt,                       /* 0x13  0xFFD8   1  ivVspi0       unused by PE */
  Cpu_Interrupt,                       /* 0x12  0xFFDA   1  ivVtimpaie    unused by PE */
  Cpu_Interrupt,                       /* 0x11  0xFFDC   1  ivVtimpaaovf  unused by PE */
  Cpu_Interrupt,                       /* 0x10  0xFFDE   1  ivVtimovf     unused by PE */
  Cpu_Interrupt,                       /* 0x0F  0xFFE0   1  ivVtimch7     unused by PE */
  Cpu_Interrupt,                       /* 0x0E  0xFFE2   1  ivVtimch6     unused by PE */
  Cpu_Interrupt,                       /* 0x0D  0xFFE4   1  ivVtimch5     unused by PE */
  Cpu_Interrupt,                       /* 0x0C  0xFFE6   1  ivVtimch4     unused by PE */
  Cpu_Interrupt,                       /* 0x0B  0xFFE8   1  ivVtimch3     unused by PE */
  Cpu_Interrupt,                       /* 0x0A  0xFFEA   1  ivVtimch2     unused by PE */
  Cpu_Interrupt,                       /* 0x09  0xFFEC   1  ivVtimch1     unused by PE */
  Cpu_Interrupt,                       /* 0x08  0xFFEE   1  ivVtimch0     unused by PE */
  TI1_Interrupt,                       /* 0x07  0xFFF0   1  ivVrti        used by PE */
  IRQ_Interrupt,                       /* 0x06  0xFFF2   1  ivVirq        unused by PE */
  Cpu_Interrupt,                       /* 0x05  0xFFF4   -  ivVxirq       unused by PE */
  Cpu_Interrupt,                       /* 0x04  0xFFF6   -  ivVswi        unused by PE */
  Cpu_Interrupt,                       /* 0x03  0xFFF8   -  ivVtrap       unused by PE */
  /* Reset handler name */
  _Startup,                         /* 0x02  0xFFFA   -  ivVcop        unused by PE */
  _Startup,                         /* 0x01  0xFFFC   -  ivVclkmon     unused by PE */
  _Startup                          /* 0x00  0xFFFE   -  ivVreset      used by PE */
};
#endif

