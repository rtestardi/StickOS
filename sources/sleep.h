// *** sleep.h ********************************************************

#if PICTOCRYPT
INTERRUPT
void
sleep_isr(void);

void
sleep_delay(int secs);

void
sleep_delay_off(void);

void
sleep_poll(void);

void
sleep_initialize(void);
#endif

