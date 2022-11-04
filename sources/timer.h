// *** timer.h ********************************************************

#if ! PIC32
INTERRUPT
void
timer_isr(void);
#endif

#if PICTOCRYPT || MC9S08QE128 || MC9S12DT256 || MC9S12DP512
enum { ticks_per_msec = 1 }; // tunable
#else
enum { ticks_per_msec = 4 }; // tunable
#endif

extern volatile int32 ticks;  // incremented by pit0 isr every tick
extern volatile int32 msecs;  // incremented by pit0 isr every millisecond
extern volatile int32 seconds;  // incremented by pit0 isr every second

extern bool volatile timer_in_isr;

#if STICK_GUEST
void
timer_ticks(bool align);
#endif

void
timer_initialize(void);

