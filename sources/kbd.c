#include "main.h"

#if KBD

static bool init;

char kbd_chars[KBDSCANS*KBDRETURNS+1] = "123a456b789c*0#d";

/*
 430 rem --- sub kdbscan ---
 440 sub kbdscan
 450   dim si, ri, b
 460   for si = 0 to 3
 470     let s[si] = 0
 480     for ri = 0 to 3
 490       let b = si*4+ri
 500       if !r[ri] then
 510         if button!=b then
 520           let button = b
 530           lcd n%2*0x40, chars$[b:1], "        "
 550         endif
 560       else
 570         if button==b then
 580           let button = -1
 590         endif
 600       endif
 610     next
 620     let s[si] = 1
 630   next
 640 endsub
*/

int32 kbd_keychar;

static void kbdinit(void)
{
    int i;
    int32 j;

    for (i = pin_assignment_kbd_s0; i < pin_assignment_kbd_s0+KBDSCANS; i++) {
        pin_set(pin_assignments[i], pin_type_digital_output, 0, 1);
    }
    
    for (i = 0; i < sizeof(kbd_chars)/4; i++) {
        j = var_get_flash(FLASH_KBDCHARS0+i);
        if (! j || j == -1) {
            break;
        }
        write32(kbd_chars+i*4, j);
    }
}

void kbd_timer_poll(void)
{
    int s;
    int r;
    int n;
    static int32 keychar;

    if (pin_assignments[pin_assignment_kbd_s0] == PIN_UNASSIGNED) {
        return;
    }

    if (! init) {
        kbdinit();
        init = true;
    }

    for (s = 0; s < KBDSCANS; s++) {
        pin_set(pin_assignments[pin_assignment_kbd_s0+s], pin_type_digital_output, 0, 0);
        for (r = 0; r < KBDRETURNS; r++) {
            n = s*4+r;
            if (! pin_get(pin_assignments[pin_assignment_kbd_r0+r], 0, 0)) {
                if (! keychar) {
                    keychar = kbd_chars[n];
                    kbd_keychar = keychar;
                }
            } else {
                if (keychar == kbd_chars[n]) {
                    keychar = 0;
                }
            }
        }
        pin_set(pin_assignments[pin_assignment_kbd_s0+s], pin_type_digital_output, 0, 1);
    }
}


#endif

