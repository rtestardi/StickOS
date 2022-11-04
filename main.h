// *** main.h *********************************************************

#ifndef MAIN_INCLUDED
#define SLEEP_DELAY  60

#include "config.h"

#if ! _WIN32
#define NULL ((void*)0)
#endif

#if ! STICK_GUEST

#if EXTRACT && ! MCF51JM128
#include "extract.h"
#else
#if MCF52233
#include "MCF52235.h"
#elif MCF52221
#include "MCF52221.h"
#elif MCF52259
#include "MCF52259.h"
#elif MCF5211
#include "MCF5211.h"
#define MCF_INTC0_ICR01  MCF_INTC_ICR01
#define MCF_INTC0_ICR13  MCF_INTC_ICR13
#define MCF_INTC0_ICR55  MCF_INTC_ICR55
#define MCF_INTC0_IMRH  MCF_INTC_IMRH
#define MCF_INTC0_IMRL  MCF_INTC_IMRL
#elif MCF51JM128
#include "MCF51JM128.h"
#include "compat.h"
#elif MCF51CN128
#include "MCF51CN128.h"
#include "compat.h"
#elif MCF51QE128
#include "MCF51QE128.h"
#include "compat.h"
#elif MC9S08QE128
#include "MC9S08QE128.h"
#include "compat.h"
#elif MC9S12DT256
#include "MC9S12DT256.h"
#include "compat.h"
#elif MC9S12DP512
#include "MC9S12DP512.h"
#include "compat.h"
#elif PIC32
#include <plib.h>
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
#else
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
#endif
#endif

#if MC9S08QE128 || MC9S12DT256 || MC9S12DP512
#if MC9S08QE128
#define MCU_CORE_BITS 8
#elif MC9S12DT256 || MC9S12DP512
#define MCU_CORE_BITS 16
#endif
typedef long intptr;
typedef unsigned long uintptr;
typedef uint16 size_t;
#else
#define MCU_CORE_BITS 32
typedef int intptr;
typedef unsigned int uintptr;
#if ! PIC32
typedef uint32 size_t;
#endif
#endif

#if GCC
#define INTERRUPT __attribute__((interrupt))
#define far
#define __IPSBAR ((volatile uint8 *)0x40000000)
extern uint8 __RAMBAR[];
#define RAMBAR_ADDRESS ((uintptr)__RAMBAR)
#define FLASH_UPGRADE_RAM_BEGIN __attribute__((section(".text_flash_upgrade_ram_begin")))
#define FLASH_UPGRADE_RAM_END __attribute__((section(".text_flash_upgrade_ram_end")))

#define asm_halt() __asm__("halt")
#define asm_stop_2000(x) __asm__("stop #0x2000")
#define asm_stop_2700(x) __asm__("stop #0x2700")

#elif MCF52221 || MCF52233 || MCF52259 || MCF5211 || MCF51JM128 || MCF51CN128 || MCF51QE128
#if MCF51JM128 || MCF51CN128 || MCF51QE128
#define INTERRUPT  interrupt
#else
#define INTERRUPT  __declspec(interrupt)
#endif
#define asm_halt() asm { halt }
#define asm_stop_2000() asm { stop #0x2000 }
#define asm_stop_2700() asm { stop #0x2700 }
#elif MC9S08QE128
#define INTERRUPT
#define asm_halt()  asm("bgnd");
#elif MC9S12DT256 || MC9S12DP512
#define INTERRUPT
#define asm_halt()  asm("bgnd");
#elif PIC32
#define INTERRUPT
#define asm_halt()  asm("SDBBP 0");
#else
#error
#endif

#else  // STICK_GUEST

#define INTERRUPT

#if WIN32
// _DEBUG/NDEBUG win
#if _DEBUG
#define SODEBUG  1
#else
#if NDEBUG
#define SODEBUG  0
#else
#error _DEBUG/NDEBUG?
#endif
#endif  // _DEBUG
#else  // WIN32
// SODEBUG wins
#if SODEBUG
#define _DEBUG
#undef NDEBUG
#else
#define NDEBUG
#undef _DEBUG
#endif  // SODEBUG
#endif  // WIN32

#if GCC
#include <inttypes.h>
#include <bits/wordsize.h>
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef int32_t int32;
typedef uint32_t uint32;
typedef uintptr_t uintptr;
typedef intptr_t intptr;
#if __WORDSIZE == 64
typedef uint64_t size_t;
#else
typedef uint32 size_t;
#endif
#else // ! GCC
#define _WIN32_WINNT 0x0601
#include <windows.h>
extern int isatty(int);
#if ! NO_UINT_TYPEDEFS
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef  int int32;
typedef unsigned int uint32;
typedef int intptr;
typedef unsigned int uintptr;
#endif
#define NO_UINT_TYPEDEFS  1
#endif // ! GCC
#include <assert.h>
#define ASSERT(x)  assert(x)
#define assert_ram(x)  assert(x)

extern void write(int, const void *, size_t);
extern char *gets(char *);

#define inline
#undef MAX_PATH

#endif  // ! STICK_GUEST

typedef unsigned char bool;
#if (! MCF51JM128 && ! MCF51CN128 && ! MCF51QE128) || GCC
typedef unsigned char byte;
#endif
typedef unsigned int uint;

enum {
    false,
    true
};

#define IN
#define OUT
#define OPTIONAL
#define VARIABLE  1
#define MIN(a, b)  ((a) < (b) ? (a) : (b))
#define MAX(a, b)  ((a) > (b) ? (a) : (b))
#define ROUNDUP(n, s)  (((n)+(s)-1)&~((s)-1))  // N.B. s must be power of 2!
#define ROUNDDOWN(n, s)  ((n)&~((s)-1))  // N.B. s must be power of 2!
#define LENGTHOF(a)  (sizeof(a)/sizeof(a[0]))
#define OFFSETOF(t, f)  ((int)(intptr)(&((t *)0)->f))
#define IS_POWER_OF_2(x) ((((x)-1)&(x))==0)

#define BASIC_OUTPUT_LINE_SIZE  79
#define BASIC_INPUT_LINE_SIZE  72

#if PIC32
#define vsnprintf  myvsnprintf
#define snprintf  mysnprintf
#define sprintf  mysprintf
#define vsprintf  myvsprintf
#define printf  myprintf
#endif

#include <stdarg.h>

#include "clone.h"
#include "flash.h"
#include "pin.h"
#include "printf.h"
#include "qspi.h"
#include "i2c.h"
#include "zigflea.h"
#include "terminal.h"
#include "text.h"
#include "timer.h"
#include "util.h"
#include "adc.h"
#include "led.h"
#include "serial.h"

#include "startup.h"

#if ! STICK_GUEST

#include "vectors.h"

#include "sleep.h"

#if MCF52221 || MCF52259 || MCF51JM128 || PIC32
#include "cdcacm.h"
#include "scsi.h"
#include "usb.h"
#endif

#if MCF52233
#undef MCF_EPORT_EPPDR
#undef MCF_EPORT_EPPAR
#undef MCF_EPORT_EPFR
#undef MCF_EPORT_EPIER
#define MCF_EPORT_EPPDR  MCF_EPORT0_EPPDR
#define MCF_EPORT_EPPAR  MCF_EPORT0_EPPAR
#define MCF_EPORT_EPFR  MCF_EPORT0_EPFR
#define MCF_EPORT_EPIER  MCF_EPORT0_EPIER
#endif

#if BADGE_BOARD
#include "jm.h"
#else
#if KBD
#include "kbd.h"
#endif
#if LCD
#include "lcd.h"
#endif
#endif

#endif  // ! STICK_GUEST

#if PICTOCRYPT && ! MCF51JM128
#include "pict-o-crypt.h"
#include "admin.h"
#include "aes.h"
#include "block.h"
#include "fat32.h"
#include "params.h"
#include "walkfs.h"

#elif STICKOS
#include "cpustick.h"
#include "basic.h"
#include "code.h"
#include "parse.h"
#include "run.h"
#include "vars.h"
#include "basic0.h"
#include "parse2.h"
#include "run2.h"

#if USB_HOST && STICKOS
#include "block.h"
#include "fat32.h"
#endif

#elif FLASHER
#include "flasher.h"

#endif  // PICTOCRYPT

#if MCF52233
extern void os_yield(void);
#else
#define os_yield()  // NULL
#endif

int
vprintf(const char *format, va_list ap);

#if ! STICK_GUEST

#if SODEBUG
#define assert(x)  do { if (! (x)) { led_line(__LINE__); } } while (0)
#else
#define assert(x)
#endif
#if SODEBUG || PIC32
#define assert_ram(x)  do { if (! (x)) { asm_halt(); } } while (0)
#else
#define assert_ram(x)
#endif
#define ASSERT(x)  do { if (! (x)) { led_line(__LINE__); } } while (0)
#define ASSERT_RAM(x)  do { if (! (x)) { asm_halt(); } } while (0)

#else  // STICK_GUEST

#if PICTOCRYPT
extern byte big_buffer[8192];
#else
extern byte big_buffer[768];
#endif

#endif  // ! STICK_GUEST

// If the platform doesn't provide FLASH_UPGRADE_RAM_{BEGIN,END} then provide default no-ops.
#ifndef FLASH_UPGRADE_RAM_BEGIN
#define FLASH_UPGRADE_RAM_BEGIN
#endif
#ifndef FLASH_UPGRADE_RAM_END
#define FLASH_UPGRADE_RAM_END
#endif

#define MAIN_INCLUDED  1
#endif  // MAIN_INCLUDED

