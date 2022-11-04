// *** lcd.c **********************************************************
// this file implements generic lcd functions.

#include "main.h"

#if LCD

static bool init;

#define LCDROWS  4
#define LCDCHARS  20

/*
 510 rem --- sub writeline ---
 520 sub writeline string, line
 530   dim i
 540   gosub writebyte 0x80+0x40*line, 0
 550   for i = 0 to 16
 560     if i<string# then
 570       gosub writebyte string[i], 1
 580     else
 590       gosub writebyte ' ', 1
 600     endif
 610   next
 620 endsub
 630 rem --- sub writebyte ---
 640 sub writebyte byte, rsin
 650   gosub writenib byte>>4, rsin
 660   gosub writenib byte, rsin
 670   sleep 1 ms
 680 endsub
 690 rem --- sub writenib ---
 700 sub writenib nib, rsin
 710   let d7 = nib&8, d6 = nib&4, d5 = nib&2, d4 = nib&1, rs = rsin
 720   let en = 1
 730   sleep 40 us
 740   let en = 0
 750   sleep 40 us
 760 endsub
 770 rem --- sub lcdinit ---
 780 sub lcdinit
 790   dim i
 800   let d7 = 0, d6 = 0, d5 = 0, d4 = 0, rs = 0, en = 0
 810   sleep 20 ms
 820   restore lcddata
 830   while 1 do
 840     read i
 850     if i==-1 then
 860       break
 870     endif
 880     gosub writebyte i, 0
 890     sleep 1 ms
 900   endwhile
 910 endsub
 920 label lcddata
 930 data 0x33, 0x32, 0x28, 0xc, 0x1, 0x6, -1
*/

static
void
writenib(int nib, int rsin)
{
    int i;
    
    for (i = 0; i < 4; i++) {
        pin_set(pin_assignments[pin_assignment_lcd_d4+i], pin_type_digital_output, 0, nib&(1<<i));
    }
    pin_set(pin_assignments[pin_assignment_lcd_rs], pin_type_digital_output, 0, rsin);
    pin_set(pin_assignments[pin_assignment_lcd_en], pin_type_digital_output, 0, 1);
    delay(1);
    pin_set(pin_assignments[pin_assignment_lcd_en], pin_type_digital_output, 0, 0);
    delay(1);
}

static
void
writebyte(int byte, int rsin)
{
    writenib(byte>>4, rsin);
    writenib(byte, rsin);
}

static
void
writeline(int pos, char *buffer, int n)
{
    int i;

    writebyte(0x80|pos, false);
    for (i = 0; i < n; i++) {
        if (isprint(buffer[i])) {
            writebyte(buffer[i], true);
        }
    }
}

static const byte data[] = {
    0x33, 0x32, 0x28, 0x0c, 0x01, 0x06
};

static
void
lcdinit(void)
{
    int i;

    for (i = pin_assignment_lcd_d4; i < pin_assignment_lcd_rs+1; i++) {
        pin_set(pin_assignments[i], pin_type_digital_output, 0, 1);
    }

    delay(50);
    for (i = 0; i < LENGTHOF(data); i++) {
        writebyte(data[i], false);
        delay(5);
    }
}


void
lcd(int pos, char *buffer, int n)
{
    int pos2;
    static const byte posn[LCDROWS] = { 0x80, 0xc0, 0x94, 0xd4 };

    if (! init) {
        lcdinit();
        init = true;
    }
    
    if (pos < 0x80) {
        pos2 = posn[pos&(LCDROWS-1)];
    } else {
        pos2 = pos;
    }
    
    writeline(pos2, buffer, n);
    
    if (pos < 0x80) {
        while (n++ < LCDCHARS) {
            writebyte(' ', true);
        }
    }
}

#endif
