#define KBDSCANS  4
#define KBDRETURNS  4

extern int32 kbd_keychar;

extern void kbd_timer_poll(void);

extern char kbd_chars[KBDSCANS*KBDRETURNS+1];
