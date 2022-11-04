#include "main.h"

// this file meets bootloader requirements for targets with bootloaders

#if GCC
extern void _startup(void);
#else
extern asm void _startup(void);
#endif

#if MCF51JM128
#if ! BADGE_BOARD
#error
#endif
#define USER_ENTRY_ADDRESS  0x000039C0
#elif MCF52259
#if ! DEMO_KIT
#error
#endif
#define USER_ENTRY_ADDRESS  0x00004000
#else
#error
#endif

#if GCC
__attribute__((section(".loader_entry_data")))
const byte _UserEntry[]
#else
const byte _UserEntry[] @ USER_ENTRY_ADDRESS
#endif
= {
    0x4E,
    0x71,
    0x4E,
    0xF9  // asm NOP(0x4E71), asm JMP(0x4EF9)           
};

#if GCC
__attribute__((section(".loader_entry2_data")))
void  (* const _UserEntry2[])()
#else
void  (* const _UserEntry2[])()@(USER_ENTRY_ADDRESS+4)
#endif
=
{
    _startup,
};

extern void pre_main(void);
void pre_main(void)
{
    int i;
    void **v;
    extern far void *_swvect[128];  // 4 bytes per entry

#if MCF51JM128
    
    Q3_NON_NAKED(move.l, #0x00800000,d0);
    Q3_NON_NAKED(movec, d0,vbr);

    v = (void **)0x00800000;

    for (i=0; i<128; i++) {
        v[i] = _swvect[i];
    }
#else
    Q3_NON_NAKED(move.l, #0x20000000,d0);
    Q3_NON_NAKED(movec, d0,vbr);

    v = (void **)0x20000000;

    for (i=0; i<128; i++) {
        v[i] = _swvect[i];
    }
#endif    
}
