#ifndef STARTUP_INCLUDED

#if PIC32 || MC9S08QE128 || MC9S12DT256 || MC9S12DP512 || BADGE_BOARD || DEMO_KIT
#define MCU_HAS_PAGE0 0
#else
#define MCU_HAS_PAGE0 1
#endif

#if !MCU_HAS_PAGE0 || STICK_GUEST
#define DECLSPEC_SWVECT
#define DECLSPEC_STARTUP
#define DECLSPEC_CFM
#elif GCC
#error // sourcery lcfs need update for swvect, startup, and cfm sections
#else
#pragma define_section swvect ".swvect" far_absolute R
#pragma define_section startup ".startup" far_absolute R
#pragma define_section cfm ".cfm" far_absolute R
#define DECLSPEC_SWVECT  __declspec(swvect)
#define DECLSPEC_STARTUP  __declspec(startup)
#define DECLSPEC_CFM  __declspec(cfm)
#endif

#if MCF52233
#define FLASH_START  0
#define FLASH_BYTES  (256*1024)
#define FLASH_PAGE_SIZE  2048
#define BASIC_RAM_PAGE_SIZE  2048
#define BASIC_VARS  100
#define BASIC_STORES  2
#elif MCF52221
#define FLASH_START  0
#define FLASH_BYTES  (128*1024)
#define FLASH_PAGE_SIZE  2048
#define BASIC_RAM_PAGE_SIZE  2048
#define BASIC_VARS  100
#define BASIC_STORES  2
#elif MCF52259
#define FLASH_START  0
#if FSLBOT
#define FLASH_BYTES  (512*1024)  // the smallest part we support
#else
#define FLASH_BYTES  (256*1024)  // the smallest part we support
#endif
#define FLASH_PAGE_SIZE  4096
#define BASIC_RAM_PAGE_SIZE  4096
#define BASIC_VARS  200
#if FSLBOT
#define BASIC_STORES  12
#else
#define BASIC_STORES  2
#endif
#elif MCF5211
#define FLASH_START  0
#define FLASH_BYTES  (128*1024)
#define FLASH_PAGE_SIZE  2048
#define BASIC_RAM_PAGE_SIZE  2048
#define BASIC_VARS  100
#define BASIC_STORES  2
#elif MCF51JM128
#define FLASH_START  0
#define FLASH_BYTES  (128*1024)
#define FLASH_PAGE_SIZE  1024
#define BASIC_RAM_PAGE_SIZE  1024
#define BASIC_VARS  100
#define BASIC_STORES  2
#elif MCF51CN128
#define FLASH_START  0
#define FLASH_BYTES  (128*1024)
#define FLASH_PAGE_SIZE  1024
#define BASIC_RAM_PAGE_SIZE  1024
#define BASIC_VARS  100
#define BASIC_STORES  2
#elif MCF51QE128
#define FLASH_START  0
#define FLASH_BYTES  (128*1024)
#define FLASH_PAGE_SIZE  1024
#define BASIC_RAM_PAGE_SIZE  512
#define BASIC_VARS  40
#define BASIC_STORES  2
#elif MC9S08QE128
#define FLASH_START  0L
#define FLASH_BYTES  (64*1024L)
#define FLASH_PAGE_SIZE  512
#define BASIC_RAM_PAGE_SIZE  512
#define BASIC_VARS  40
#define BASIC_STORES  2

#define FLASH2_PPAGE  0x06
#define FLASH2_START  0x8000  // BASIC stores, for code access
#define FLASH2_BYTES  (16*1024L)
#elif MC9S12DT256
#define FLASH_START  0L
#define FLASH_BYTES  (64*1024L)
#define FLASH_PAGE_SIZE  512
#define BASIC_RAM_PAGE_SIZE  512
#define BASIC_VARS  40
#define BASIC_STORES  2

#define FLASH2_PPAGE  0x3c  // N.B. must be in flash block 0!
#define FLASH2_START  0x8000  // BASIC stores, for code access
#define FLASH2_BYTES  (16*1024L)
#elif MC9S12DP512
#define FLASH_START  0L
#define FLASH_BYTES  (64*1024L)
#define FLASH_PAGE_SIZE  1024
#define BASIC_RAM_PAGE_SIZE  512
#define BASIC_VARS  40
#define BASIC_STORES  2

#define FLASH2_PPAGE  0x3c  // N.B. must be in flash block 0!
#define FLASH2_START  0x8000  // BASIC stores, for code access
#define FLASH2_BYTES  (16*1024L)
#elif PIC32
#define FLASH_START  0x9D000000
#if defined(__32MX250F128B__)
#define FLASH_BYTES  (124*1024)  // the smallest part we support
#define FLASH_PAGE_SIZE  1024
#define BASIC_RAM_PAGE_SIZE  2048
#define BASIC_VARS  100
#define BASIC_STORES  0
#define PIC32PPS  1
#elif defined(__32MX320F128H__)
#define FLASH_BYTES  (128*1024)  // the smallest part we support
#define FLASH_PAGE_SIZE  4096
#define BASIC_RAM_PAGE_SIZE  2048
#define BASIC_VARS  100
#define BASIC_STORES  0
#elif DUINOMITE
#define FLASH_BYTES  (512*1024)  // the smallest part we support
#define FLASH_PAGE_SIZE  4096
#define BASIC_RAM_PAGE_SIZE  4096
#define BASIC_VARS  200
#define BASIC_STORES  10
#else
#define FLASH_BYTES  (256*1024)  // the smallest part we support
#define FLASH_PAGE_SIZE  4096
#define BASIC_RAM_PAGE_SIZE  4096
#define BASIC_VARS  200
#define BASIC_STORES  2
#endif

#define FLASH2_START  0x9FC00000  // boot flash, for flash upgrade
#define FLASH2_BYTES  (12*1024)
#else
#error
#endif

#if MC9S08QE128 || MC9S12DT256
#define BASIC_LARGE_PAGE_SIZE  (7*1024)
#elif MC9S12DP512
#define BASIC_LARGE_PAGE_SIZE  (6*1024)
#else
#if SODEBUG && ! STICK_GUEST
#define BASIC_LARGE_PAGE_SIZE  (8*1024)
#else
#if PIC32 || MCF52233 || MCF52259
#define BASIC_LARGE_PAGE_SIZE  (24*1024)
#else
#define BASIC_LARGE_PAGE_SIZE  (12*1024)
#endif
#endif
#endif

#define BASIC_SMALL_PAGE_SIZE  FLASH_PAGE_SIZE

extern bool disable_autorun;
extern uint16 flash_checksum;
extern bool usb_host_mode;

extern byte *end_of_static;

extern uint32 cpu_frequency;
extern uint32 bus_frequency;
extern uint32 oscillator_frequency;

extern bool debugger_attached;
#if PICTOCRYPT
extern byte big_buffer[8192];
#else
extern byte big_buffer[768];
#endif

// make cw and gcc assemblers compatible
#if GCC
#define BEGIN_NAKED(func)  extern uint8 func[]; \
                           void func ## _not(void) \
                           { \
                               asm ("\t.globl " #func ); \
                               asm ("\t.type " #func ", @function"); \
                               asm (#func ":");

#define END_NAKED              asm("\thalt"); \
                           }
                           
// use Q[1-3]() within naked functions, declared with BEGIN_NAKED()/END_NAKED.
#define Q1(a)  asm("\t" #a "\n");
#define Q2(a,b)  asm("\t" #a " " #b "\n");
#define Q3(a,b,c)  asm("\t" #a " " #b "," #c "\n");
// use within a non-naked routine for inline asm.
#define Q3_NON_NAKED(a,b,c)  asm("\t" #a " " #b "," #c "\n");
#else  // GCC
#define BEGIN_NAKED(func)  asm void __declspec(register_abi) func(void)

#define END_NAKED

// use Q[1-3]() within naked functions, declared with BEGIN_NAKED()/END_NAKED.
#define Q1(a)  a
#define Q2(a,b)  a b
#define Q3(a,b,c)  a b,c
// use within a non-naked routine for inline asm.
#define Q3_NON_NAKED(a,b,c)  asm(a b,c)
#endif  // GCC

typedef void (*flash_upgrade_ram_begin_f)(void);

void
#if PIC32 && ! _WIN32
__longramfunc__
__attribute__((nomips16))
#endif
flash_upgrade_ram_begin(void);

void
flash_upgrade_ram_end(void);

#define STARTUP_INCLUDED  1
#endif  // STARTUP_INCLUDED

