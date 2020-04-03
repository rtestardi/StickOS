// *** led.h **********************************************************

#if PICTOCRYPT
enum code {
    code_battery = 1,  // 1
    code_usb  // 2
};

void
led_unknown(void);  // blue
#endif

void
led_unknown_progress(void);  // blue fast (ignored if happy)

#if PICTOCRYPT
void
led_happy(void);  // green

void
led_happy_progress(void);  // green fast

void
led_sad(enum code code);  // red running
#endif

void
led_timer_poll(void);

#if MC9S08QE128 || MC9S12DT256 || MC9S12DP512
#pragma CODE_SEG __NEAR_SEG NON_BANKED
#endif
void
led_line(int line);  // red crashed

void
led_hex(int hex);
#if MC9S08QE128 || MC9S12DT256 || MC9S12DP512
#pragma CODE_SEG DEFAULT
#endif

void
led_initialize(void);

