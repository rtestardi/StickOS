// *** jm.h **********************************************************

void
jm_timer_poll();

void
jm_set(int r, int c);

void
jm_clear(int r, int c);

void
jm_scroll(char *text, int length);

bool
jm_scroll_ready();

void
jm_initialize(void);

// revisit -- this belongs somewhere else!
void
pre_main(void);

