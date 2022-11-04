// *** vector.c *******************************************************
// this file contains page0 interrupt vectors for all interrupts.  edit
// this file to hook up __declspec(interrupt) functions to hardware
// interrupts.

#if ! PIC32 && ! MC9S08QE128 && ! MC9S12DT256 && ! MC9S12DP512
#include "main.h"

#if ! PIC32 && ! MC9S08QE128 && ! MC9S12DT256 && ! MC9S12DP512
extern unsigned char far _SP_INIT[], _SDA_BASE[];
extern unsigned char far __SP_AFTER_RESET[];
extern unsigned char far ___RAMBAR[], ___RAMBAR_SIZE[];
extern unsigned char far __DATA_RAM[], __DATA_ROM[], __DATA_END[];
#endif

// *** page0 ***

#pragma explicit_zero_data on

#if MCF52233
extern __declspec(interrupt) void fec_isr(void);
#endif

#if GCC
extern void _startup(void);
#else // GCC
asm void _startup(void);
#endif // GCC

#if GCC
extern void asm_xxx(void);
#else // GCC
asm void asm_xxx(void);
#endif // GCC

#if MCF52221 || MCF52233 || MCF52259 || MCF5211
// this is the hardware interrupt vector table.
#if ! BADGE_BOARD && ! DEMO_KIT
DECLSPEC_SWVECT
#else
const
#endif
void *_swvect[128] = {
    __SP_AFTER_RESET, _startup,
    asm_xxx,                  // 2
    asm_xxx,                  // 3
    asm_xxx,                  // 4
    asm_xxx,                  // 5
    asm_xxx,                  // 6
    asm_xxx,                  // 7
    asm_xxx,                  // 8
    asm_xxx,                  // 9
    asm_xxx,                  // 10
    asm_xxx,                  // 11
    asm_xxx,                  // 12
    asm_xxx,                  // 13
    asm_xxx,                  // 14
    asm_xxx,                  // 15
    asm_xxx,                  // 16
    asm_xxx,                  // 17
    asm_xxx,                  // 18
    asm_xxx,                  // 19
    asm_xxx,                  // 20
    asm_xxx,                  // 21
    asm_xxx,                  // 22
    asm_xxx,                  // 23
    asm_xxx,                  // 24
    asm_xxx,                  // 25
    asm_xxx,                  // 26
    asm_xxx,                  // 27
    asm_xxx,                  // 28
    asm_xxx,                  // 29
    asm_xxx,                  // 30
    asm_xxx,                  // 31
    asm_xxx,                  // 32
    asm_xxx,                  // 33
    asm_xxx,                  // 34
    asm_xxx,                  // 35
    asm_xxx,                  // 36
    asm_xxx,                  // 37
    asm_xxx,                  // 38
    asm_xxx,                  // 39
    asm_xxx,                  // 40
    asm_xxx,                  // 41
    asm_xxx,                  // 42
    asm_xxx,                  // 43
    asm_xxx,                  // 44
    asm_xxx,                  // 45
    asm_xxx,                  // 46
    asm_xxx,                  // 47
    asm_xxx,                  // 48
    asm_xxx,                  // 49
    asm_xxx,                  // 50
    asm_xxx,                  // 51
    asm_xxx,                  // 52
    asm_xxx,                  // 53
    asm_xxx,                  // 54
    asm_xxx,                  // 55
    asm_xxx,                  // 56
    asm_xxx,                  // 57
    asm_xxx,                  // 58
    asm_xxx,                  // 59
    asm_xxx,                  // 60
    asm_xxx,                  // 61
    asm_xxx,                  // 62
    asm_xxx,                  // 63
    asm_xxx,                  // 64
#if ! FLASHER && ! PICTOCRYPT && (MCF52259 || MCF5211)
    zb_isr,                   // 65 - irq1*
#else
    asm_xxx,                  // 65
#endif
    asm_xxx,                  // 66
    asm_xxx,                  // 67
#if ! FLASHER && ! PICTOCRYPT && ! MCF52259 && ! MCF5211
    zb_isr,                   // 68 - irq4*
#else
    asm_xxx,                  // 68
#endif
    asm_xxx,                  // 69
    asm_xxx,                  // 70
    asm_xxx,                  // 71
    asm_xxx,                  // 72
    asm_xxx,                  // 73
    asm_xxx,                  // 74
    asm_xxx,                  // 75
    asm_xxx,                  // 76
#if ! FLASHER && ! PICTOCRYPT
    serial_isr,               // 77 - uart0
#else
    asm_xxx,                  // 77
#endif
    asm_xxx,                  // 78
    asm_xxx,                  // 79
    asm_xxx,                  // 80
    asm_xxx,                  // 81
    asm_xxx,                  // 82
    asm_xxx,                  // 83
    asm_xxx,                  // 84
    asm_xxx,                  // 85
    asm_xxx,                  // 86
#if MCF52233
    fec_isr,                  // 87 ifec.c
    fec_isr,                  // 88 ifec.c
    fec_isr,                  // 89 ifec.c
    fec_isr,                  // 90 ifec.c
    fec_isr,                  // 91 ifec.c
    fec_isr,                  // 92 ifec.c
    fec_isr,                  // 93 ifec.c
    fec_isr,                  // 94 ifec.c
    fec_isr,                  // 95 ifec.c
    fec_isr,                  // 96 ifec.c
    fec_isr,                  // 97 ifec.c
    fec_isr,                  // 98 ifec.c
    fec_isr,                  // 99 ifec.c
#else
    asm_xxx,                  // 87
    asm_xxx,                  // 88
    asm_xxx,                  // 89
    asm_xxx,                  // 90
    asm_xxx,                  // 91
    asm_xxx,                  // 92
    asm_xxx,                  // 93
    asm_xxx,                  // 94
    asm_xxx,                  // 95
    asm_xxx,                  // 96
    asm_xxx,                  // 97
    asm_xxx,                  // 98
    asm_xxx,                  // 99
#endif
    asm_xxx,                  // 100
    asm_xxx,                  // 101
    asm_xxx,                  // 102
    asm_xxx,                  // 103
    asm_xxx,                  // 104
    asm_xxx,                  // 105
    asm_xxx,                  // 106
    asm_xxx,                  // 107
    asm_xxx,                  // 108
    asm_xxx,                  // 109
    asm_xxx,                  // 110
    asm_xxx,                  // 111
    asm_xxx,                  // 112
    asm_xxx,                  // 113
    asm_xxx,                  // 114
    asm_xxx,                  // 115
    asm_xxx,                  // 116
#if (MCF52221 || MCF52259) && ! FLASHER
    usb_isr,                  // 117 usb.c
#else
    asm_xxx,                  // 117
#endif
    asm_xxx,                  // 118
    timer_isr,                // 119 timer.c
    asm_xxx,                  // 120
    asm_xxx,                  // 121
    asm_xxx,                  // 122
    asm_xxx,                  // 123
    asm_xxx,                  // 124
    asm_xxx,                  // 125
    asm_xxx,                  // 126
    asm_xxx,                  // 127
};
#elif MCF51JM128 || MCF51CN128 || MCF51QE128
// this is the hardware interrupt vector table.
#if ! BADGE_BOARD && ! DEMO_KIT
DECLSPEC_SWVECT
#else
const
#endif
void *_swvect[128] = {
    __SP_AFTER_RESET, _startup,
    asm_xxx,                  // 2
    asm_xxx,                  // 3
    asm_xxx,                  // 4
    asm_xxx,                  // 5
    asm_xxx,                  // 6
    asm_xxx,                  // 7
    asm_xxx,                  // 8
    asm_xxx,                  // 9
    asm_xxx,                  // 10
    asm_xxx,                  // 11
    asm_xxx,                  // 12
    asm_xxx,                  // 13
    asm_xxx,                  // 14
    asm_xxx,                  // 15
    asm_xxx,                  // 16
    asm_xxx,                  // 17
    asm_xxx,                  // 18
    asm_xxx,                  // 19
    asm_xxx,                  // 20
    asm_xxx,                  // 21
    asm_xxx,                  // 22
    asm_xxx,                  // 23
    asm_xxx,                  // 24
    asm_xxx,                  // 25
    asm_xxx,                  // 26
    asm_xxx,                  // 27
    asm_xxx,                  // 28
    asm_xxx,                  // 29
    asm_xxx,                  // 30
    asm_xxx,                  // 31
    asm_xxx,                  // 32
    asm_xxx,                  // 33
    asm_xxx,                  // 34
    asm_xxx,                  // 35
    asm_xxx,                  // 36
    asm_xxx,                  // 37
    asm_xxx,                  // 38
    asm_xxx,                  // 39
    asm_xxx,                  // 40
    asm_xxx,                  // 41
    asm_xxx,                  // 42
    asm_xxx,                  // 43
    asm_xxx,                  // 44
    asm_xxx,                  // 45
    asm_xxx,                  // 46
    asm_xxx,                  // 47
    asm_xxx,                  // 48
    asm_xxx,                  // 49
    asm_xxx,                  // 50
    asm_xxx,                  // 51
    asm_xxx,                  // 52
    asm_xxx,                  // 53
    asm_xxx,                  // 54
    asm_xxx,                  // 55
    asm_xxx,                  // 56
    asm_xxx,                  // 57
    asm_xxx,                  // 58
    asm_xxx,                  // 59
    asm_xxx,                  // 60
    asm_xxx,                  // 61
    asm_xxx,                  // 62
    asm_xxx,                  // 63
    zb_pre_isr,               // 64 zigflea.c
    asm_xxx,                  // 65
    asm_xxx,                  // 66
    asm_xxx,                  // 67
    asm_xxx,                  // 68
#if MCF51JM128
    usb_isr,                  // 69 usb.c
#else
    asm_xxx,                  // 69
#endif
    asm_xxx,                  // 70
    asm_xxx,                  // 71
    asm_xxx,                  // 72
    asm_xxx,                  // 73
    asm_xxx,                  // 74
    asm_xxx,                  // 75
    asm_xxx,                  // 76
#if MCF51QE128
    serial_isr,               // 77 serial.c
#else
    asm_xxx,                  // 77
#endif
    asm_xxx,                  // 78
    asm_xxx,                  // 79
    asm_xxx,                  // 80
    asm_xxx,                  // 81
#if MCF51JM128
    serial_isr,               // 82 serial.c
#else
    asm_xxx,                  // 82
#endif
#if MCF51CN128
    serial_isr,               // 83 serial.c
#else
    asm_xxx,                  // 83
#endif
    asm_xxx,                  // 84
    asm_xxx,                  // 85
#if MCF51QE128
    timer_isr,                // 86 timer.c
#else
    asm_xxx,                  // 86
#endif
    asm_xxx,                  // 87
    asm_xxx,                  // 88
    asm_xxx,                  // 89
    asm_xxx,                  // 90
#if MCF51JM128
    timer_isr,                // 91 timer.c
#else
    asm_xxx,                  // 91
#endif
    asm_xxx,                  // 92
    asm_xxx,                  // 93
    asm_xxx,                  // 94
    asm_xxx,                  // 95
    asm_xxx,                  // 96
    asm_xxx,                  // 97
    asm_xxx,                  // 98
#if MCF51QE128
    zb_isr,                   // 99 zigflea.c
#else
    asm_xxx,                  // 99
#endif
    asm_xxx,                  // 100
    asm_xxx,                  // 101
    asm_xxx,                  // 102
    asm_xxx,                  // 103
    asm_xxx,                  // 104
    asm_xxx,                  // 105
#if MCF51CN128
    zb_isr,                   // 106 zigflea.c
#else
    asm_xxx,                  // 106
#endif
#if MCF51JM128
    zb_isr,                   // 107 zigflea.c
#else
    asm_xxx,                  // 107
#endif
    asm_xxx,                  // 108
    asm_xxx,                  // 109
    asm_xxx,                  // 110
    asm_xxx,                  // 111
    asm_xxx,                  // 112
    asm_xxx,                  // 113
#if MCF51CN128
    timer_isr,                // 114 timer.c
#else
    asm_xxx,                  // 114
#endif
    asm_xxx,                  // 115
    asm_xxx,                  // 116
    asm_xxx,                  // 117
    asm_xxx,                  // 118
    asm_xxx,                  // 119
    asm_xxx,                  // 120
    asm_xxx,                  // 121
    asm_xxx,                  // 122
    asm_xxx,                  // 123
    asm_xxx,                  // 124
    asm_xxx,                  // 125
    asm_xxx,                  // 126
    asm_xxx,                  // 127
};
#else
#error
#endif

// this is the cfm config
// XXX -- this should have #error for unspecified MCUs!
#if ! BADGE_BOARD && ! DEMO_KIT
DECLSPEC_CFM
#else
const
#endif
uint32 _cfm[] = {
#if ! MCF51JM128 && ! MCF51CN128 && ! MCF51QE128
    0,                              // (0x00000400) KEY_UPPER
    0,                              // (0x00000404) KEY_LOWER
    0xffffffff,                     // (0x00000408) CFMPROT
    0,                              // (0x0000040C) CFMSACC
    0,                              // (0x00000410) CFMDACC
#if SECURE
    MCF_CFM_CFMSEC_SECSTAT|0x4AC8,  // (0x00000414) CFMSEC
#else
    0,                              // (0x00000414) CFMSEC
#endif
#else
    0,                              // 400 key
    0,                              // 404 key
    0,                              // 408 reserved
    0x00ff0000                      // 40c prot/opt
#endif
};
#endif

