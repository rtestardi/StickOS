// *** parse2.c *******************************************************
// this file implements private extensions to the stickos bytecode
// compiler and de-compiler.

#include "main.h"

static
const struct keyword {
    char *keyword;
    enum bytecode2 code;
} keywords[] = {
#if BADGE_BOARD
    "jmscroll", code_scroll,
    "jmset", code_set,
    "jmclear", code_clear,
#else
#if LCD
    "lcd", code_lcd,
#endif
#endif
    "private", code_private,
};


// this function parses (compiles) a private statement line to bytecode.
bool
parse2_line(IN char *text_in, OUT int *length_out, OUT byte *bytecode, OUT int *syntax_error_in)
{
    int i;
    int len;
    char *text;
    int length;
#if BADGE_BOARD || LCD
    bool boo;
    int length2;
    int syntax_error;
#endif

    text = text_in;
    parse_trim(&text);

    for (i = 0; i < LENGTHOF(keywords); i++) {
        len = strlen(keywords[i].keyword);
        if (! strncmp(text, keywords[i].keyword, len)) {
            text += len;
            break;
        }
    }
    if (i == LENGTHOF(keywords) || i == code_private) {
        goto XXX_ERROR_XXX;
    }

    parse_trim(&text);

    length = 0;
    bytecode[length++] = keywords[i].code;

    switch (keywords[i].code) {
#if BADGE_BOARD
        case code_scroll:
            boo = parse_line_code(code_print, text, &length2, bytecode+length, &syntax_error);
            if (! boo) {
                *syntax_error_in = text - text_in + syntax_error;
                assert(*syntax_error_in >= 0 && *syntax_error_in < BASIC_OUTPUT_LINE_SIZE);
                return boo;
            }
            *length_out = length+length2;
            return true;
            break;

        case code_set:
        case code_clear:
            // parse the row expression
            if (! parse_expression(0, &text, &length, bytecode)) {
                goto XXX_ERROR_XXX;
            }
            
            if (! parse_char(&text, ',')) {
                goto XXX_ERROR_XXX;
            }
            bytecode[length++] = code_comma;
            
            // parse the column expression
            if (! parse_expression(0, &text, &length, bytecode)) {
                goto XXX_ERROR_XXX;
            }
            
            if (*text) {
                goto XXX_ERROR_XXX;
            }
            break;
#else
#if LCD
        case code_lcd:
            // parse the pos expression
            if (! parse_expression(0, &text, &length, bytecode)) {
                goto XXX_ERROR_XXX;
            }
            
            if (! parse_char(&text, ',')) {
                goto XXX_ERROR_XXX;
            }
            bytecode[length++] = code_comma;

            boo = parse_line_code(code_print, text, &length2, bytecode+length, &syntax_error);
            if (! boo) {
                *syntax_error_in = text - text_in + syntax_error;
                assert(*syntax_error_in >= 0 && *syntax_error_in < BASIC_OUTPUT_LINE_SIZE);
                return boo;
            }
            *length_out = length+length2;
            return true;
            break;
#endif
#endif
        default:
            assert(0);
            break;
    }

    if (*text) {
        goto XXX_ERROR_XXX;
    }

    *length_out = length;
    return true;

XXX_ERROR_XXX:
    *syntax_error_in = text - text_in;
    assert(*syntax_error_in >= 0 && *syntax_error_in < BASIC_OUTPUT_LINE_SIZE);
    return false;
}

// this function unparses (de-compiles) a private statement line from bytecode.
void
unparse2_bytecode(IN byte *bytecode_in, IN int length, OUT char *text)
{
    int i;
    byte code;
    char *out;
    byte *bytecode;

    bytecode = bytecode_in;
    out = text;

    code = *bytecode++;

    // find the bytecode keyword
    for (i = 0; i < LENGTHOF(keywords); i++) {
        if (keywords[i].code == code) {
            break;
        }
    }
    assert(i != LENGTHOF(keywords));

    // decompile the bytecode keyword
    out += sprintf(out, keywords[i].keyword);
    out += sprintf(out, " ");

    switch (code) {
#if BADGE_BOARD
        case code_scroll:
            unparse_bytecode_code(code_print, bytecode, bytecode_in+length-bytecode, out);
            return;
            break;

        case code_set:
        case code_clear:
            bytecode += unparse_expression(0, bytecode, bytecode_in+length-bytecode, &out);
            
            out += sprintf(out, ", ");
            assert(*bytecode == code_comma);
            bytecode++;
            
            bytecode += unparse_expression(0, bytecode, bytecode_in+length-bytecode, &out);
            break;
#else
#if LCD
        case code_lcd:
            bytecode += unparse_expression(0, bytecode, bytecode_in+length-bytecode, &out);
            
            out += sprintf(out, ", ");
            assert(*bytecode == code_comma);
            bytecode++;

            unparse_bytecode_code(code_print, bytecode, bytecode_in+length-bytecode, out);
            return;
            break;
#endif
#endif
        default:
            assert(0);
            break;
    }

    assert(bytecode == bytecode_in+length);
}

