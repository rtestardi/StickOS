#define VERSION  "1.92g"

//#define SODEBUG  1
#define DEBUGGING  1  // enable to use mplab x debugger

#define FSLBOT  0
// #define DEMO_KIT  1  // and change to demokit.lcf and check bootload.c to build

#if MCF52221 || MCF52233 || MCF52259 || MCF5211
#define DEMO  1  // 1 enables DEMO board USB power
#endif

#if PIC32
#define STARTER  1  // 1 enables STARTER board USB power
#endif

#if PICTOCRYPT
#define SECURE  0  // 1 sets flash security
#else
#define SECURE  0  // 1 sets flash security
#endif

#ifndef EXTRACT
#define EXTRACT  0  // 1 uses extracted headers rather than Freescale
#endif

#if ! BADGE_BOARD
#define KBD  1
#define LCD  1
#endif

#if MCF52221 || MCF52259 || MCF51JM128 || (PIC32 && ! defined(__32MX320F128H__))
#define USBOTG  1
#endif

#if MCF52259 || (PIC32 && ! defined(__32MX320F128H__))
#define USB_HOST  1
#endif

#define ZIGFLEA  1

#if PIC32 && ! defined(__32MX320F128H__) && ! defined(__32MX440F512H__) && ! defined(__32MX795F512H__)
#define PIC32PORTA  1
#endif

#if ! CHIPKIT
#if ! BADGE_BOARD && ! DEMO_KIT && ! MCF9S08QE128 && ! MC9S12DT256 && ! MC9S12DP512 && ! MC51QE128
#define UPGRADE  1
#endif
#if ! PIC32
#define DOWNLOAD  1
#endif
#endif

// Enable in-memory trace buffer for debugging with the trace() macro.
#define IN_MEMORY_TRACE 0

