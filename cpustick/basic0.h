// *** basic0.h *******************************************************

enum bytecode2 {
    code_private = code_max,
#if BADGE_BOARD
    code_scroll,
    code_set,
    code_clear,
#else
#if LCD
    code_lcd,
#endif
#endif
    code_max_max
};

// revisit -- move to var2.h???
enum flash_var2 {
    FLASH_UNUSED = FLASH_NEXT,
#if MCF52233
    FLASH_IPADDRESS,
#endif
#if KBD
    FLASH_KBDCHARS0,
    FLASH_KBDCHARS1,
    FLASH_KBDCHARS2,
    FLASH_KBDCHARS3,
#endif
    FLASH_LAST_LAST
};

extern char * const help_about;

#if BADGE_BOARD
extern char * const help_about_short;
#endif

#if MC9S08QE128 || MC9S12DT256 || MC9S12DP512
#pragma CODE_SEG HELP_CODE
#endif

void basic0_help(IN char *text_in);

#if MC9S08QE128 || MC9S12DT256 || MC9S12DP512
#pragma CODE_SEG DEFAULT
#endif

void basic0_run(char *text_in);

void basic0_poll(void);

