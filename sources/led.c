// *** led.c **********************************************************
// this file implements basic LED controls.

#include "main.h"

enum led {
#if PICTOCRYPT
    led_red = 0,  // dtin0
    led_green,  // dtin1
    led_blue,  // dtin2
#else
    led_red = 0,  // dtin3
    led_blue = 0,  // dtin3
#endif
    led_max
};

static int led_count[led_max];
static int last_led_count[led_max];

#if PICTOCRYPT
static int led_code;
#endif

static enum led led_blink;


#if PICTOCRYPT
void
led_unknown(void)
{
    led_code = 0;
    led_blink = led_blue;
}
#endif

void
led_unknown_progress(void)
{
    led_count[led_blue]++;
    
#if PICTOCRYPT
    // go to sleep SLEEP_DELAY seconds after progress stops
    sleep_delay(SLEEP_DELAY);
#endif
}

#if PICTOCRYPT
void
led_happy(void)
{
    led_code = 0;
    led_blink = led_green;
}

void
led_happy_progress(void)
{
    led_count[led_green]++;
}

void
led_sad(enum code code)
{
    led_code = code;
    led_blink = led_red;
}
#endif

#if MC9S08QE128 || MC9S12DT256 || MC9S12DP512
#pragma CODE_SEG __NEAR_SEG NON_BANKED
#endif
// this function turns an LED on or off.
static
void
led_set(enum led n, int on)
{
    assert (n >= 0 && n < led_max);

#if ! STICK_GUEST
#if PICTOCRYPT
    // red LED workaround; tri-state when off!
    if (on) {
        MCF_GPIO_DDRTC = 1 << n;
    } else {
        MCF_GPIO_DDRTC = 0;
    }

    MCF_GPIO_SETTC = (uint8)~(1 << n);
    if (on) {
        MCF_GPIO_CLRTC = (uint8)~(1 << n);
    } else {
        MCF_GPIO_SETTC = (uint8)(1 << n);
    }
#else
    pin_set(pin_assignments[pin_assignment_heartbeat], pin_type_digital_output, 0, on);
#endif  // PICTOCRYPT
#endif // ! STICK_GUEST
}
#if MC9S08QE128 || MC9S12DT256 || MC9S12DP512
#pragma CODE_SEG DEFAULT
#endif

void
led_timer_poll()
{
#if PICTOCRYPT
    int i;
    static bool green;
#endif
    static int calls;
    
    calls++;
    
#if PICTOCRYPT
    // if we have a run-time error code to display...
    if (led_code) {
        i = calls%(led_code*2+12);
        if (i >= led_code*2) {
            led_set(led_blink, 0);
        } else {
            led_set(led_blink, i%2);
        }
        
        return;
    }
    
    // if we have a green blip to display...
    if (green || (calls%8 == 0 && led_count[led_green] != last_led_count[led_green])) {
        green = true;
        led_set((calls/2)%2?led_green:led_blink, calls%2);  // blink green fast
        if (calls%8 == 7) {
            green = false;
        }
        last_led_count[led_green] = led_count[led_green];
        return;
    }
#endif

    // normal led processing
    if (led_count[led_blink] != last_led_count[led_blink]) {
        led_set(led_blink, calls%2);  // blink fast
        last_led_count[led_blink] = led_count[led_blink];
    } else {
        led_set(led_blink, (calls/4)%2);  // blink slow
    }
}

#if ! STICK_GUEST

#if MC9S08QE128 || MC9S12DT256 || MC9S12DP512
#pragma CODE_SEG __NEAR_SEG NON_BANKED
#endif
// this function displays a diagnostic code on a LED.
void
led_line(int line)
{
    int i;
    int j;
    int n;
    int inv;
    
    if (debugger_attached) {
        asm_halt();
    } else {
        splx(7);
        
        n = 0;
        inv = 0;
        for (;;) {
            if (n++ >= 10) {
#if PICTOCRYPT
                sleep_delay(0);
                sleep_poll();
#endif
            }
            
            for (i = 1000; i > 0; i /= 10) {
                if (line < i) {
                    continue;
                }
                j = (line/i)%10;
                if (! j) {
                    j = 10;
                }
                while (j--) {
                    led_set(led_red, 1^inv);
                    delay(200);
                    led_set(led_red, 0^inv);
                    delay(200);
                }
                delay(1000);
            }
            delay(3000);
            inv = ! inv;
            led_set(led_red, 0^inv);
            delay(3000);
        }
    }
}

// this function displays a diagnostic code on a LED.
void
led_hex(int hex)
{
    int i;
    int j;
    int n;
    int inv;
    
    if (debugger_attached) {
        asm_halt();
    } else {
        splx(7);
        
        n = 0;
        inv = 0;
        for (;;) {
            if (n++ >= 10) {
#if PICTOCRYPT
                sleep_delay(0);
                sleep_poll();
#endif
            }
            
            for (i = 0x10000000; i > 0; i /= 16) {
                if (hex < i) {
                    continue;
                }
                j = (hex/i)%16;
                if (! j) {
                    j = 16;
                }
                while (j--) {
                    led_set(led_red, 1^inv);
                    delay(200);
                    led_set(led_red, 0^inv);
                    delay(200);
                }
                delay(1000);
            }
            delay(3000);
            inv = ! inv;
            led_set(led_red, 0^inv);
            delay(3000);
        }
    }
}
#if MC9S08QE128 || MC9S12DT256 || MC9S12DP512
#pragma CODE_SEG DEFAULT
#endif

#endif // ! STICK_GUEST

// this function initializes the led module.
void
led_initialize(void)
{
#if ! STICK_GUEST
#if PICTOCRYPT
    // dtin0, 1, 2 are digital output
    MCF_GPIO_PTCPAR = 0;
    MCF_GPIO_DDRTC = 0x7;
    MCF_GPIO_CLRTC = ~0x7;  // all LEDs on to indicate reset!
#endif
#endif // ! STICK_GUEST
}

