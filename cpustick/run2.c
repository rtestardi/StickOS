// *** run2.c *********************************************************
// this file implements private extensions to the stickos bytecode
// execution engine.

#include "main.h"

#if BADGE_BOARD
bool run2_scroll;
#endif
#if LCD
bool run2_lcd;
#endif

// this function executes a private bytecode statement.
bool  // end
run2_bytecode_code(uint code, const byte *bytecode, int length)
{
#if BADGE_BOARD
    int r;
    int c;
#endif
#if LCD
    int32 pos;
#endif
#if BADGE_BOARD || LCD
    bool boo;
#endif
    bool end;
    int index;

    end = false;

    index = 0;

    switch (code) {
#if BADGE_BOARD
        case code_scroll:
#if ! GCC
            cw7bug++;  // CW7 BUG
#endif

#if ! STICK_GUEST
            while (! jm_scroll_ready()) {
                // see if the sleep switch was pressed
                basic0_poll();
            }
#endif

            run2_scroll = true;
            boo = run_bytecode_code(code_print, run_condition, bytecode, length);
            run2_scroll = false;

            return boo;

        case code_set:
        case code_clear:
            index += run_expression(bytecode+index, length-index, NULL, &r);

            assert(bytecode[index] == code_comma);
            index++;

            index += run_expression(bytecode+index, length-index, NULL, &c);

            if (run_condition) {
#if ! STICK_GUEST
                if (code == code_set) {
                    jm_set(r, c);
                } else {
                    jm_clear(r, c);
                }
#endif
            }
            break;
#else
#if LCD
        case code_lcd:
#if ! GCC
            cw7bug++;  // CW7 BUG
#endif
            index += run_expression(bytecode+index, length-index, NULL, &pos);

            assert(bytecode[index] == code_comma);
            index++;

            run2_lcd = pos+1;
            boo = run_bytecode_code(code_print, run_condition, bytecode+index, length-index);
            run2_lcd = 0;

            return boo;
#endif
#endif

        default:
            assert(0);
            break;
    }

    assert(index == length);
    return end;

//XXX_SKIP_XXX:
    stop();
    return false;
}

