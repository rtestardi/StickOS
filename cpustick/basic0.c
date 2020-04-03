// *** basic0.c *******************************************************
// this file implements private extensions to the stickos command
// interpreter.

#include "main.h"

enum cmdcode {
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    command_clone,  // [run]
#endif
    command_connect,  // nnn
#if STICK_GUEST || SODEBUG
    command_demo,
#endif
#if DOWNLOAD
    command_download,  // nnn
#endif
    command_help,
#if MCF52233
    command_ipaddress,  // [dhcp|<ipaddress>]
#endif
#if KBD
    command_keychars,  // 0123456789abcdef
#endif
    command_nodeid,  // nnn
    command_reset,
#if UPGRADE
    command_upgrade,
#endif
    command_uptime,
#if ZIGFLEA
    command_zigflea,
#endif
    command_dummy
};

static
const char * const commands[] = {
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    "clone",
#endif
    "connect",
#if STICK_GUEST || SODEBUG
    "demo",
#endif
#if DOWNLOAD
    "download",
#endif
    "help",
#if MCF52233
    "ipaddress",
#endif
#if KBD
    "keychars",
#endif
    "nodeid",
    "reset",
#if UPGRADE
    "upgrade",
#endif
    "uptime",
#if ZIGFLEA
    "zigflea",
#endif
};


// *** help ***********************************************************

#if BADGE_BOARD
char * const help_about_short =
"StickOS for MCF51JM128 v" VERSION "!";
#endif

char * const help_about =
#if MCF52233
"Welcome to StickOS for Freescale MCF52233 v" VERSION "!\n"
#elif MCF52221
"Welcome to StickOS for Freescale MCF52221 v" VERSION "!\n"
#elif DEMO_KIT && MCF52259
"Welcome to StickOS for Freescale MCF52252 DemoKit v" VERSION "!\n"
#elif FSLBOT && MCF52259
"Welcome to StickOS for Freescale MCF52259 v" VERSION "!\n"
#elif MCF52259
"Welcome to StickOS for Freescale MCF52252 v" VERSION "!\n"
#elif MCF5211
"Welcome to StickOS for Freescale MCF5211 v" VERSION "!\n"
#elif BADGE_BOARD && MCF51JM128
"Welcome to StickOS for Freescale MCF51JM128 BadgeBoard v" VERSION "!\n"
#elif MCF51JM128 && FB32
"Welcome to StickOS for Freescale MCF51JM128 Firebird32 v" VERSION "!\n"
#elif MCF51JM128 && FB32LITE
"Welcome to StickOS for Freescale MCF51JM128 Firebird32-lite v" VERSION "!\n"
#elif MCF51JM128
"Welcome to StickOS for Freescale MCF51JM128 v" VERSION "!\n"
#elif MCF51CN128
"Welcome to StickOS for Freescale MCF51CN128 v" VERSION "!\n"
#elif MCF51QE128
"Welcome to StickOS for Freescale MCF51QE128 v" VERSION "!\n"
#elif MC9S08QE128
"Welcome to StickOS for Freescale MC9S08QE128 v" VERSION "!\n"
#elif MC9S12DT256
"Welcome to StickOS for Freescale MC9S12DT256 v" VERSION "!\n"
#elif MC9S12DP512
"Welcome to StickOS for Freescale MC9S12DP512 v" VERSION "!\n"
#elif PIC32 && defined(__32MX250F128B__)
"Welcome to StickOS for Microchip PIC32MX2-F128B v" VERSION "!\n"
#elif PIC32 && defined(__32MX320F128H__) && defined CHIPKIT
"Welcome to StickOS for Microchip PIC32MXx-F128H chipKIT Uno32 v" VERSION "!\n"
#elif PIC32 && defined(__32MX440F512H__) && HIDBL
"Welcome to StickOS for Microchip PIC32MX4-F512H CUI32 v" VERSION "!\n"
#elif PIC32 && defined(__32MX440F512H__)
"Welcome to StickOS for Microchip PIC32MXx-F512H v" VERSION "!\n"
#elif PIC32 && defined(__32MX460F512L__) && HIDBL
"Welcome to StickOS for Microchip PIC32MXx-F512L UBW32v1 v" VERSION "!\n"
#elif PIC32 && defined(__32MX460F512L__)
"Welcome to StickOS for Microchip PIC32MXx-F512L v" VERSION "!\n"
#elif PIC32 && defined(__32MX795F512H__) && DUINOMITE
"Welcome to StickOS for Microchip PIC32MX7-F512H DuinoMite-Mega v" VERSION "!\n"
#elif PIC32 && defined(__32MX795F512H__) && HIDBL
"Welcome to StickOS for Microchip PIC32MX7-F512H CUI32Stem v" VERSION "!\n"
#elif PIC32 && defined(__32MX795F512L__) && defined CHIPKIT
"Welcome to StickOS for Microchip PIC32MX7-F512L chipKIT Max32 v" VERSION "!\n"
#elif PIC32 && defined(__32MX795F512L__) && HIDBL
"Welcome to StickOS for Microchip PIC32MX7-F512L UBW32v2 v" VERSION "!\n"
#elif PIC32 && defined(__32MX795F512H__)
"Welcome to StickOS for Microchip PIC32MX7-F512H v" VERSION "!\n"
#elif PIC32 && defined(__32MX795F512L__)
"Welcome to StickOS for Microchip PIC32MX7-F512L v" VERSION "!\n"
#else
#error
#endif
"Copyright (c) 2008-2012; all rights reserved.  Patent U.S. 8,117,587.\n"
"http://www.cpustick.com\n"
"support@cpustick.com\n"
;

#if MC9S08QE128 || MC9S12DT256 || MC9S12DP512
#pragma CODE_SEG HELP_CODE
#pragma STRING_SEG HELP_STRING
// N.B. for MC9S08QE128/MC9S12DT256/MC9S12DP512 we put all the help strings
// and the function that accesses them in page 7 or 38; we have to put the
// functions that copy these to RAM in unbanked memory.
#endif


/*
to change help:

1. change the help inside the "dead" part of the #if (between GENERATE-HELP-BEGIN and GENERATE-HELP-END)
2. download the Win32 version of StickOS from cpustick.com
3. in a shell window, cd to the cpustick directory
4. run: "StickOS.exe help <basic0.c >help.c"
5. rebuild
*/
#if 1
#include "help.c"
#else
// GENERATE_HELP_BEGIN

#if ! SODEBUG || STICK_GUEST
static char *const help_general =
"for more information:\n"
"  help about\n"
"  help commands\n"
"  help modes\n"
"  help statements\n"
"  help blocks\n"
"  help devices\n"
"  help expressions\n"
"  help strings\n"
"  help variables\n"
"  help pins\n"
#if MCF52259
"  help board\n"
#endif
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
"  help clone\n"
#endif
"  help zigflea\n"
"\n"
"see also:\n"
"  http://www.cpustick.com\n"
;

static char * const help_commands =
"<Ctrl-C>                      -- stop program\n"
"auto <line>                   -- automatically number program lines\n"
"clear [flash]                 -- clear ram [and flash] variables\n"
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
"clone [run]                   -- clone flash to slave MCU [and run]\n"
#endif
"cls                           -- clear terminal screen\n"
"cont [<line>]                 -- continue program from stop\n"
"delete ([<line>][-][<line>]|<subname>) -- delete program lines\n"
#if DOWNLOAD
"download <slave Hz>           -- download flash to slave MCU\n"
#endif
"dir                           -- list saved programs\n"
"edit <line>                   -- edit program line\n"
"help [<topic>]                -- online help\n"
"list ([<line>][-][<line>]|<subname>) -- list program lines\n"
"load <name>                   -- load saved program\n"
"memory                        -- print memory usage\n"
"new                           -- erase code ram and flash memories\n"
"profile ([<line>][-][<line>]|<subname>) -- display profile info\n"
"purge <name>                  -- purge saved program\n"
"renumber [<line>]             -- renumber program lines (and save)\n"
"reset                         -- reset the MCU!\n"
"run [<line>]                  -- run program\n"
"save [<name>|library]         -- save code ram to flash memory\n"
"subs                          -- list sub names\n"
"undo                          -- undo code changes since last save\n"
#if UPGRADE
"upgrade                       -- upgrade StickOS firmware!\n"
#endif
"uptime                        -- print time since last reset\n"
"\n"
"for more information:\n"
"  help modes\n"
;

static char * const help_modes =
"analog [<millivolts>]             -- set/display analog voltage scale\n"
"baud [<rate>]                     -- set/display uart console baud rate\n"
"autorun [on|off]                  -- autorun mode (on reset)\n"
"echo [on|off]                     -- terminal echo mode\n"
"indent [on|off]                   -- listing indent mode\n"
#if MCF52233
"ipaddress [dhcp|<ipaddress>]      -- set/display ip address\n"
#endif
#if KBD
"keychars [<keychars>]             -- set/display keypad scan chars\n"
#endif
"nodeid [<nodeid>|none]            -- set/display zigflea nodeid\n"
"numbers [on|off]                  -- listing line numbers mode\n"
"pins [<assign> [<pinname>|none]]  -- set/display StickOS pin assignments\n"
"prompt [on|off]                   -- terminal prompt mode\n"
"servo [<Hz>]                      -- set/display servo Hz (on reset)\n"
"step [on|off]                     -- debugger single-step mode\n"
"trace [on|off]                    -- debugger trace mode\n"
#if USB_HOST
"usbhost [on|off]                  -- USB host mode (on reset)\n"
#endif
"watchsmart [on|off]               -- low-overhead watchpoint mode\n"
"\n"
"pin assignments:\n"
"  heartbeat  safemode*\n"
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
"  qspi_cs*  clone_rst*  zigflea_rst*  zigflea_attn*  zigflea_rxtxen\n"
#else
"  qspi_cs*  zigflea_rst*  zigflea_attn*  zigflea_rxtxen\n"
#endif
"\n"
"for more information:\n"
"  help pins\n"
;

static char * const help_statements =
"<line>                                 -- delete program line from code ram\n"
"<line> <statement>  // comment         -- enter program line into code ram\n"
"\n"
"<variable>[$] = <expression> [, ...]   -- assign variable\n"
"? [dec|hex|raw] <expression> [, ...] [;] -- print results\n"
"assert <expression>                    -- break if expression is false\n"
"data <n> [, ...]                       -- read-only data\n"
"dim <variable>[$][[n]] [as ...] [, ...] -- dimension variables\n"
"end                                    -- end program\n"
#if BADGE_BOARD
"jm(clear|set) <r>, <c>                 -- clear/set row/column of LED matrix\n"
"jmscroll ...                           -- scroll printout to LED matrix\n"
#endif
"halt                                   -- loop forever\n"
"input [dec|hex|raw] <variable>[$] [, ...] -- input data\n"
"label <label>                          -- read/data label\n"
#if LCD
"lcd <pos>, [dec|hex|raw] <expression> [, ...] [;] -- display results on lcd\n"
#endif
"let <variable>[$] = <expression> [, ...] -- assign variable\n"
"print [dec|hex|raw] <expression> [, ...] [;] -- print results\n"
"read <variable> [, ...]                -- read read-only data into variables\n"
"rem <remark>                           -- remark\n"
"restore [<label>]                      -- restore read-only data pointer\n"
"sleep <expression> (s|ms|us)           -- delay program execution\n"
"stop                                   -- insert breakpoint in code\n"
"vprint <variable>[$] = [dec|hex|raw] <expression> [, ...] -- print to variable\n"
"\n"
"for more information:\n"
"  help blocks\n"
"  help devices\n"
"  help expressions\n"
"  help strings\n"
"  help variables\n"
;

static char * const help_blocks =
"if <expression> then\n"
"[elseif <expression> then]\n"
"[else]\n"
"endif\n"
"\n"
"for <variable> = <expression> to <expression> [step <expression>]\n"
"  [(break|continue) [n]]\n"
"next\n"
"\n"
"while <expression> do\n"
"  [(break|continue) [n]]\n"
"endwhile\n"
"\n"
"do\n"
"  [(break|continue) [n]]\n"
"until <expression>\n"
"\n"
"gosub <subname> [<expression>, ...]\n"
"\n"
"sub <subname> [<param>, ...]\n"
"  [return]\n"
"endsub\n"
;

static char * const help_devices =
"timers:\n"
"  configure timer <n> for <n> (s|ms|us)\n"
"  on timer <n> do <statement>                -- on timer execute statement\n"
"  off timer <n>                              -- disable timer interrupt\n"
"  mask timer <n>                             -- mask/hold timer interrupt\n"
"  unmask timer <n>                           -- unmask timer interrupt\n"
"\n"
"uarts:\n"
"  configure uart <n> for <n> baud <n> data (even|odd|no) parity [loopback]\n"
"  on uart <n> (input|output) do <statement>  -- on uart execute statement\n"
"  off uart <n> (input|output)                -- disable uart interrupt\n"
"  mask uart <n> (input|output)               -- mask/hold uart interrupt\n"
"  unmask uart <n> (input|output)             -- unmask uart interrupt\n"
"  uart <n> (read|write) <variable> [, ...]   -- perform uart I/O\n"
"\n"
"i2c:\n"
"  i2c (start <addr>|(read|write) <variable> [, ...]|stop) -- master i2c I/O\n"
"\n"
"qspi:\n"
"  qspi <variable> [, ...]                    -- master qspi I/O\n"
"\n"
"watchpoints:\n"
"  on <expression> do <statement>             -- on expr execute statement\n"
"  off <expression>                           -- disable expr watchpoint\n"
"  mask <expression>                          -- mask/hold expr watchpoint\n"
"  unmask <expression>                        -- unmask expr watchpoint\n"
;

static char * const help_expressions =
"the following operators are supported as in C,\n"
"in order of decreasing precedence:\n"
"  <n>                       -- decimal constant\n"
"  0x<n>                     -- hexadecimal constant\n"
"  'c'                       -- character constant\n"
"  <variable>                -- simple variable\n"
"  <variable>[<expression>]  -- array variable element\n"
"  <variable>#               -- length of array or string\n"
"  (   )                     -- grouping\n"
"  !   ~                     -- logical not, bitwise not\n"
"  *   /   %                 -- times, divide, mod\n"
"  +   -                     -- plus, minus\n"
"  >>  <<                    -- shift right, left\n"
"  <=  <  >=  >              -- inequalities\n"
"  ==  !=                    -- equal, not equal\n"
"  |   ^   &                 -- bitwise or, xor, and\n"
"  ||  ^^  &&                -- logical or, xor, and\n"
"for more information:\n"
"  help variables\n"
;

static char * const help_strings =
"v$ is a nul-terminated view into a byte array v[]\n"
"\n"
"string statements:\n"
"  dim, input, let, print, vprint\n"
"  if <expression> <relation> <expression> then\n"
"  while <expression> <relation> <expression> do\n"
"  until <expression> <relation> <expression> do\n"
"\n"
"string expressions:\n"
"  \"literal\"                      -- literal string\n"
"  <variable>$                    -- variable string\n"
"  <variable>$[<start>:<length>]  -- variable substring\n"
"  +                              -- concatenates strings\n"
"\n"
"string relations:\n"
"  <=  <  >=  >                   -- inequalities\n"
"  ==  !=                         -- equal, not equal\n"
"  ~  !~                          -- contains, does not contain\n"
"for more information:\n"
"  help variables\n"
;

static char *const help_variables =
"all variables must be dimensioned!\n"
"variables dimensioned in a sub are local to that sub\n"
"simple variables are passed to sub params by reference; otherwise, by value\n"
"array variable indices start at 0\n"
"v is the same as v[0], except for input/print/i2c/qspi/uart statements\n"
"\n"
"ram variables:\n"
"  dim <var>[$][[n]]\n"
"  dim <var>[[n]] as (byte|short)\n"
"\n"
"absolute variables:\n"
"  dim <var>[[n]] [as (byte|short)] at address <addr>\n"
"\n"
"flash parameter variables:\n"
"  dim <varflash>[[n]] as flash\n"
"\n"
"pin alias variables:\n"
"  dim <varpin> as pin <pinname> for (digital|analog|servo|frequency|uart) \\\n"
"                                      (input|output) \\\n"
"                                      [debounced] [inverted] [open_drain]\n"
"\n"
"system variables (read-only):\n"
"  analog  getchar"
#if KBD
"  keychar"
#endif
"  msecs  nodeid\n"
"  random  seconds  ticks  ticks_per_msec\n"
"\n"
"for more information:\n"
"  help pins\n"
;

static char *const help_pins =
"pin names:\n"
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
"    0         1         2         3        4     5        6        7\n"
"  --------  --------- --------- -------- ----- -------- -------- ------+\n"
"  an0       an1       an2       an3      an4   an5      an6      an7   | AN\n"
"  scl       sda                                                        | AS\n"
#if MCF52233 || MCF52259 || MCF5211
"  gpt0      gpt1      gpt2      gpt3                                   | TA\n"
#endif
#if MCF52259
"            irq1*               irq3*          irq5*             irq7* | NQ\n"
#else
"            irq1*                        irq4*                   irq7* | NQ\n"
#endif
#if MCF52233
"                                irq11*                                 | GP\n"
#endif
"  qspi_dout qspi_din  qspi_clk  qspi_cs0       qspi_cs2 qspi_cs3       | QS\n"
"  dtin0     dtin1     dtin2     dtin3                                  | TC\n"
"  utxd0     urxd0     urts0*    ucts0*                                 | UA\n"
"  utxd1     urxd1     urts1*    ucts1*                                 | UB\n"
#if MCF52259
"  fec_col   fec_crs   fec_rxclk fec_rxd[0-3]                  fec_rxdv | TI\n"
"  fec_rxer  fec_txclk fec_txd[0-3]                   fec_txen fec_txer | TJ\n"
"  utxd2     urxd2     urts2*    ucts2*                                 | UC\n"
#endif
"\n"
"all pins support general purpose digital input/output\n"
"an? = potential analog input pins (mV)\n"
#if MCF52233 || MCF52259 || MCF5211
"dtin?, gpt? = potential analog output (PWM) pins (mV)\n"
"dtin?, gpt? = potential servo output (PWM) pins (us)\n"
#else
"dtin? = potential analog output (PWM) pins (mV)\n"
"dtin? = potential servo output (PWM) pins (us)\n"
#endif
"dtin? = potential frequency output pins (Hz)\n"
"urxd? = potential uart input pins (received byte)\n"
"utxd? = potential uart output pins (transmit byte)\n"
#elif MCF51JM128
#if FB32
"  a0-a7, 0-13\n"
"  pta[0-5], ptc[0-1,3-6], ptd[0,3-7], pte7, ptf[5,7], ptg[0-2]\n"
"\n"
"peripherals:\n"
"  switch                 ptg0\n"
"  buzzer                 ptf5\n"
"  rgb anode (+)          ptg1\n"
"  rgb red cathode (-)    5 (pte3)\n"
"  rgb green cathode (-)  3 (pte2)\n"
"  rgb blue cathode (-)   9 (ptf1)\n"
"  eeprom write protect   ptg2\n"
"\n"
"jumpers:\n"
"  d6=pwm, d10=pwm\n"
"\n"
"all pins support general purpose digital input/output\n"
"a? = potential analog input pins (mV)\n"
"3,5,6,8,9,10,11 = potential analog output (PWM) pins (mV)\n"
"3,5,6,8,9,10,11 = potential servo output (PWM) pins (us)\n"
"3,5,6,8,9,10,11 = potential frequency output pins (Hz)\n"
#else
"    0       1       2       3       4       5       6       7\n"
"  ------- ------- ------- ------- ------- ------- ------- --------+\n"
"  pta0    pta1    pta2    pta3    pta4    pta5                    | PORT A\n"
"  ptb0    ptb1    ptb2    ptb3    ptb4    ptb5    ptb6    ptb7    | PORT B\n"
"  ptc0    ptc1    ptc2    ptc3    ptc4    ptc5    ptc6            | PORT C\n"
"  ptd0    ptd1    ptd2    ptd3    ptd4    ptd5    ptd6    ptd7    | PORT D\n"
"  pte0    pte1    pte2    pte3    pte4    pte5    pte6    pte7    | PORT E\n"
"  ptf0    ptf1    ptf2    ptf3    ptf4    ptf5    ptf6    ptf7    | PORT F\n"
"  ptg0    ptg1    ptg2    ptg3                                    | PORT G\n"
"\n"
"all pins support general purpose digital input/output\n"
"ptb?, ptd[0134] = potential analog input pins (mV)\n"
"pte[23], ptf[0-5] = potential analog output (PWM) pins (mV)\n"
"pte[23], ptf[0-5] = potential servo output (PWM) pins (us)\n"
"pte[23], ptf[0-5] = potential frequency output pins (Hz)\n"
"pte1 (u1), ptc5 (u2) = potential uart input pins (received byte)\n"
"pte0 (u1), ptc3 (u2) = potential uart output pins (transmit byte)\n"
#endif
#elif MCF51CN128
"    0       1       2       3       4       5       6       7\n"
"  ------- ------- ------- ------- ------- ------- ------- --------+\n"
"  pta0    pta1    pta2    pta3    pta4    pta5    pta6    pta7    | PORT A\n"
"  ptb0    ptb1    ptb2    ptb3    ptb4    ptb5    ptb6    ptb7    | PORT B\n"
"  ptc0    ptc1    ptc2            ptc4    ptc5    ptc6    ptc7    | PORT C\n"
"  ptd0    ptd1    ptd2    ptd3                    ptd6    ptd7    | PORT D\n"
"  pte0    pte1    pte2    pte3    pte4    pte5    pte6    pte7    | PORT E\n"
"  ptf0    ptf1    ptf2    ptf3    ptf4    ptf5    ptf6    ptf7    | PORT F\n"
"  ptg0    ptg1    ptg2    ptg3    ptg4    ptg5    ptg6    ptg7    | PORT G\n"
"  pth0    pth1    pth2    pth3    pth4    pth5    pth6    pth7    | PORT H\n"
"  ptj0    ptj1    ptj2    ptj3    ptj4    ptj5                    | PORT J\n"
"\n"
"all pins support general purpose digital input/output\n"
"ptc[4-7], ptd[0-37], pte[0-2] = potential analog input pins (mV)\n"
"ptb[67], ptc0, pte[3-5] = potential analog output (PWM) pins (mV)\n"
"ptb[67], ptc0, pte[3-5] = potential servo output (PWM) pins (us)\n"
"ptb[67], ptc0, pte[3-5] = potential frequency output pins (Hz)\n"
"ptd1 (u1), ptd3 (u2), pta4 (u3) = potential uart input pins (received byte)\n"
"ptd0 (u1), ptd2 (u2), pta3 (u3) = potential uart output pins (transmit byte)\n"
#elif MCF51QE128 || MC9S08QE128
"    0       1       2       3       4       5       6       7\n"
"  ------- ------- ------- ------- ------- ------- ------- --------+\n"
"  pta0    pta1    pta2    pta3    pta4    pta5    pta6    pta7    | PORT A\n"
"  ptb0    ptb1    ptb2    ptb3    ptb4    ptb5    ptb6    ptb7    | PORT B\n"
"  ptc0    ptc1    ptc2    ptc3    ptc4    ptc5    ptc6    ptc7    | PORT C\n"
"  ptd0    ptd1    ptd2    ptd3    ptd4    ptd5    ptd6    ptd7    | PORT D\n"
"  pte0    pte1    pte2    pte3    pte4    pte5    pte6    pte7    | PORT E\n"
"  ptf0    ptf1    ptf2    ptf3    ptf4    ptf5    ptf6    ptf7    | PORT F\n"
"  ptg0    ptg1    ptg2    ptg3                                    | PORT G\n"
"\n"
"all pins support general purpose digital input/output\n"
"pta[0-367], ptb[0-3], ptf[0-7], ptg[23] = potential analog input pins (mV)\n"
"pta[0167], ptb[45], ptc[0-5] = potential analog output (PWM) pins (mV)\n"
"pta[0167], ptb[45], ptc[0-5] = potential servo output (PWM) pins (us)\n"
"pta[0167], ptb[45], ptc[0-5] = potential frequency output pins (Hz)\n"
"ptb0 (u1), ptc6 (u2) = potential uart input pins (received byte)\n"
"ptb1 (u1), ptc7 (u2) = potential uart output pins (transmit byte)\n"
#elif MC9S12DT256 || MC9S12DP512
"    0       1       2       3       4       5       6       7\n"
"  ------- ------- ------- ------- ------- ------- ------- --------+\n"
"  pad00   pad01   pad02   pad03   pad04   pad05   pad06   pad07   | PORT AD0\n"
"  pad08   pad09   pad10   pad11   pad12   pad13   pad14   pad15   | PORT AD1\n"
"  pa0     pa1     pa2     pa3     pa4     pa5     pa6     pa7     | PORT A\n"
"  pb0     pb1     pb2     pb3     pb4     pb5     pb6     pb7     | PORT B\n"
"  pe0     pe1     pe2     pe3     pe4     pe5     pe6     pe7     | PORT E\n"
"  ph0     ph1     ph2     ph3     ph4     ph5     ph6     ph7     | PORT H\n"
"  pj0     pj1                                     pj6     pj7     | PORT J\n"
"  pk0     pk1     pk2     pk3     pk4     pk5     pk6     pk7     | PORT K\n"
"  pm0     pm1     pm2     pm3     pm4     pm5     pm6     pm7     | PORT M\n"
"  pp0     pp1     pp2     pp3     pp4     pp5     pp6     pp7     | PORT P\n"
"  ps0     ps1     ps2     ps3     ps4     ps5     ps6     ps7     | PORT S\n"
"  pt0     pt1     pt2     pt3     pt4     pt5     pt6     pt7     | PORT T\n"
"\n"
"all pins support general purpose digital input\n"
"all pins except pad?? and pe[01] support general purpose digital output\n"
"pad?? = potential analog input pins (mV)\n"
"pp? = potential analog output (PWM) pins (mV)\n"
"pp? = potential servo output (PWM) pins (us)\n"
"pt? = potential frequency output pins (Hz)\n"
"ps0 (u0), ps2 (u1) = potential uart input pins (received byte)\n"
"ps1 (u0), ps3 (u1) = potential uart output pins (transmit byte)\n"
#elif PIC32
#if defined CHIPKIT
#if defined(__32MX320F128H__)
"  a0-a11, 0-13, 26-41, 43, rg2, rg3, rg9\n"
"\n"
"jumpers:\n"
"  jp5,jp7=master, jp4=rd4\n"
#else
"  a0-a15, 0-13, 16-23, 28-39, 43-49, 52-53, 70-85\n"
"\n"
"pin aliases:\n"
"  40=a11, 41=a13, 42=a12, 50=29, 51=43\n"
"\n"
"jumpers:\n"
"  jp3,jp4=master\n"
#endif
"\n"
"all pins support general purpose digital input/output\n"
"a? = potential analog input pins (mV)\n"
"3,5,6,9,10 = potential analog output (PWM) pins (mV)\n"
"3,5,6,9,10 = potential servo output (PWM) pins (us)\n"
"3,5,6,9,10 = potential frequency output pins (Hz)\n"
#else
"  0/8     1/9     2/10    3/11    4/12    5/13    6/14    7/15\n"
"  ------- ------- ------- ------- ------- ------- ------- --------+\n"
#if PIC32PORTA
"  ra0     ra1     ra2     ra3     ra4     ra5     ra6     ra7     | PORT A\n"
"          ra9     ra10                            ra14    ra15    |      A+8\n"
#endif
"  an0     an1     an2     an3     an4     an5     an6     an7     | PORT B\n"
"  an8     an9     an10    an11    an12    an13    an14    an15    |      B+8\n"
"          rc1     rc2     rc3     rc4                             | PORT C\n"
"                                  rc12    rc13    rc14    rc15    |      C+8\n"
"  rd0     rd1     rd2     rd3     rd4     rd5     rd6     rd7     | PORT D\n"
"  rd8     rd9     rd10    rd11    rd12    rd13    rd14    rd15    |      D+8\n"
"  re0     re1     re2     re3     re4     re5     re6     re7     | PORT E\n"
"  re8     re9                                                     |      E+8\n"
"  rf0     rf1     rf2     rf3     rf4     rf5                     | PORT F\n"
"  rf8                             rf12    rf13                    |      F+8\n"
"  rg0     rg1     rg2     rg3                     rg6     rg7     | PORT G\n"
"  rg8     rg9                     rg12    rg13    rg14    rg15    |      G+8\n"
"\n"
"all pins support general purpose digital input/output\n"
"an? = potential analog input pins (mV)\n"
"rd[0-4] = potential analog output (PWM) pins (mV)\n"
"rd[0-4] = potential servo output (PWM) pins (us)\n"
"rd[0-4] = potential frequency output pins (Hz)\n"
"rf4 (u2) = potential uart input pins (received byte)\n"
"rf5 (u2) = potential uart output pins (transmit byte)\n"
#endif
#else
#error
#endif
;

#if MCF52259
static char *const help_board =
"1  2  3  4  5  6  7  8  9  10 11 12    1  2  3  4  5  6  7  8  9  10 11 12\n"
"\n"
"g  +  q  q  q  q  r  r  q  q  r  v     u  u  u  u  g  g  g  g  i  i  i  i\n"
"n  3  s  s  s  s  c  s  s  s  s  b     t  r  r  c  p  p  p  p  r  r  r  r\n"
"d  V  p  p  p  p  o  t  p  p  t  u     x  x  t  t  t  t  t  t  q  q  q  q\n"
"      i  i  i  i  n  i  i  i  o  s     d  d  s  s  0  1  2  3  1  3  5  7\n"
"      _  _  _  _  *  n  _  _  u        2  2  2  2              *  *  *  *\n"
"      c  d  d  c     *  c  c  t              *  *                        \n"
"      l  o  i  s        s  s  *                                          \n"
"      k  u  n  0        2  3                                             \n"
"         t                                                               \n"
"\n"
"antenna                            MCU                                USB\n"
"\n"
"\n"
"      u  u        u  u                                                   \n"
"u  u  r  c  u  u  r  c  d  d  d  d                                       \n"
"t  r  t  t  t  r  t  t  t  t  t  t                                       \n"
"x  x  s  s  x  x  s  s  i  i  i  i     g  +  s  s  a  a  a  a  a  a  a  a\n"
"d  d  0  0  d  d  1  1  n  n  n  n     n  3  c  d  n  n  n  n  n  n  n  n\n"
"0  0  *  *  1  1  *  *  0  1  2  3     d  V  l  a  0  1  2  3  4  5  6  7\n"
"\n"
"1  2  3  4  5  6  7  8  9  10 11 12    1  2  3  4  5  6  7  8  9  10 11 12\n"
;
#endif

#if MCF52221 || MCF52233 || MCF52259 || MCF5211
static char *const help_clone =
"clone cable:\n"
"  master           slave\n"
"  ---------        ----------------\n"
"  qspi_clk         qspi_clk (ezpck)\n"
"  qspi_din         qspi_dout (ezpq)\n"
"  qspi_dout        qspi_din (ezpd)\n"
"  qspi_cs0         rcon* (ezpcs*)\n"
"  pins clone_rst*  rsti*\n"
"  vss              vss\n"
"  vdd              vdd\n"
;
#endif

static char *const help_zigflea =
"connect <nodeid>              -- connect to MCU <nodeid> via zigflea\n"
"<Ctrl-D>                      -- disconnect from zigflea\n"
"\n"
"remote node variables:\n"
"  dim <varremote>[[n]] as remote on nodeid <nodeid>\n"
"\n"
"zigflea cable:\n"
"  MCU                  MC1320X\n"
"  -------------        -----------\n"
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
"  qspi_clk             spiclk\n"
"  qspi_din             miso\n"
"  qspi_dout            mosi\n"
#if MCF52259
"  irq1*                irq*\n"
#else
"  irq4*                irq*\n"
#endif
#elif MCF51JM128
"  spsck1 (pte6)        spiclk\n"
"  miso1 (pte4)         miso\n"
"  mosi1 (pte5)         mosi\n"
"  irq*                 irq*\n"
#elif MCF51CN128
"  spsck1 (ptb5)        spiclk\n"
"  miso1 (ptb4)         miso\n"
"  mosi1 (ptb3)         mosi\n"
"  irq* (ptc4)          irq*\n"
#elif MCF51QE128 || MC9S08QE128
"  spsck1 (ptb2)        spiclk\n"
"  miso1 (ptb4)         miso\n"
"  mosi1 (ptb3)         mosi\n"
"  irq*                 irq*\n"
#elif MC9S12DT256 || MC9S12DP512
"  sck0 (pm5)           spiclk\n"
"  miso0 (pm2)          miso\n"
"  mosi0 (pm4)          mosi\n"
"  irq* (pe1)           irq*\n"
#elif PIC32
// REVISIT -- implement zigflea on MRF24J40
"  sck1                 spiclk\n"
"  sdi1                 miso\n"
"  sdo1                 mosi\n"
"  int1                 irq*\n"
#else
#error
#endif
"  pins qspi_cs*        ce*\n"
"  pins zigflea_rst*    rst*\n"
"  pins zigflea_rxtxen  rxtxen\n"
"  vss                  vss\n"
"  vdd                  vdd\n"
;
#endif

// GENERATE_HELP_END
#endif

void
basic0_help(IN char *text_in)
{
    char *p;
    char *text;
    byte line[BASIC_OUTPUT_LINE_SIZE];
    char line2[BASIC_OUTPUT_LINE_SIZE];

    text = text_in;

    // while there is more help to print...
    while (*text) {
        // print the next line of help
        p = strchr(text, '\n');
        assert(p);
        assert(p-text < BASIC_OUTPUT_LINE_SIZE);
        memcpy(line, text, p-text);
#if MC9S08QE128 || MC9S12DT256 || MC9S12DP512
#pragma STRING_SEG DEFAULT
#endif
        line[p-text] = '\0';
        text_expand(line, line2);
        printf("%s\n", line2);
        text = p+1;
    }

#if ! STICK_GUEST
    if (text_in == help_about) {
        printf("(checksum 0x%x)\n", flash_checksum);
#if 0
        printf("MCF_RCM_RSR = 0x%x\n", MCF_RCM_RSR);
#endif
    }
#endif
}

#if MC9S08QE128 || MC9S12DT256 || MC9S12DP512
#pragma CODE_SEG DEFAULT
#endif


// *** demo ***********************************************************
#if STICK_GUEST || SODEBUG
static const char * const demos[] = {
  "rem ### blinky ###\n"
#if STICK_GUEST
  "dim i\n"
#endif
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
  "dim led as pin dtin2 for digital output\n"
#elif MCF51JM128
  "dim led as pin ptf1 for digital output inverted\n"
#elif MCF51CN128
  "dim led as pin ptg5 for digital output inverted\n"
#elif MCF51QE128 || MC9S08QE128
  "dim led as pin ptc3 for digital output inverted\n"
#elif MC9S12DT256 || MC9S12DP512
  "dim led as pin pb6 for digital output inverted\n"
#elif PIC32
  "dim led as pin rd2 for digital output inverted\n"
#else
#error
#endif
#if STICK_GUEST
  "while 1 do\n"
  "  for i = 1 to 16\n"
  "    let led = !led\n"
  "    sleep 50 ms\n"
  "  next\n"
  "  sleep 800 ms\n"
  "endwhile\n"
  "end\n"
#endif
,
  "rem ### uart isr ###\n"
#if STICK_GUEST
  "dim data\n"
  "data 1, 1, 2, 3, 5, 8, 13, 21, 0\n"
#endif
#if MCF52221 || MCF52233 || MCF52259 || MCF5211 || MC9S12DT256 || MC9S12DP512
  "configure uart 0 for 300 baud 8 data no parity loopback\n"
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
  "dim tx as pin utxd0 for uart output\n"
  "dim rx as pin urxd0 for uart input\n"
#elif MC9S12DT256 || MC9S12DP512
  "dim tx as pin ps1 for uart output\n"
  "dim rx as pin ps0 for uart input\n"
#else
#error
#endif
  "on uart 0 input do gosub receive\n"
  "on uart 0 output do gosub transmit\n"
#else
  "configure uart 2 for 300 baud 8 data no parity loopback\n"
#if MCF51JM128
  "dim tx as pin ptc3 for uart output\n"
  "dim rx as pin ptc5 for uart input\n"
#elif MCF51CN128
  "dim tx as pin ptd2 for uart output\n"
  "dim rx as pin ptd3 for uart input\n"
#elif MCF51QE128 || MC9S08QE128
  "dim tx as pin ptc7 for uart output\n"
  "dim rx as pin ptc6 for uart input\n"
#elif PIC32
  "dim tx as pin rf5 for uart output\n"
  "dim rx as pin rf4 for uart input\n"
#else
#error
#endif
  "on uart 2 input do gosub receive\n"
  "on uart 2 output do gosub transmit\n"
#endif
#if STICK_GUEST
  "sleep 1000 ms\n"
  "end\n"
  "sub receive\n"
  "  print \"received\", rx\n"
  "endsub\n"
  "sub transmit\n"
  "  read data\n"
  "  if ! data then\n"
  "    return\n"
  "  endif\n"
  "  assert !tx\n"
  "  print \"sending\", data\n"
  "  let tx = data\n"
  "endsub\n"
#endif
,
  "rem ### uart pio ###\n"
#if STICK_GUEST
  "configure uart 1 for 9600 baud 7 data even parity loopback\n"
#endif
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
  "dim tx as pin utxd1 for uart output\n"
  "dim rx as pin urxd1 for uart input\n"
#elif MCF51JM128
  "dim tx as pin pte0 for uart output\n"
  "dim rx as pin pte1 for uart input\n"
#elif MCF51CN128
  "dim tx as pin ptd0 for uart output\n"
  "dim rx as pin ptd1 for uart input\n"
#elif MCF51QE128 || MC9S08QE128
  "dim tx as pin ptb1 for uart output\n"
  "dim rx as pin ptb0 for uart input\n"
#elif MC9S12DT256 || MC9S12DP512
  "dim tx as pin ps3 for uart output\n"
  "dim rx as pin ps2 for uart input\n"
#elif PIC32
  "dim tx as pin rf8 for uart output\n"
  "dim rx as pin rf2 for uart input\n"
#else
#error
#endif
#if STICK_GUEST
  "let tx = 3\n"
#if MCF51JM128 || MCF51CN128 || MCF51QE128 || MC9S08QE128 || MC9S12DT256 || MC9S12DP512
  "while tx do\n"
  "endwhile\n"
  "print rx\n"
#endif
  "let tx = 4\n"
  "while tx do\n"
  "endwhile\n"
  "print rx\n"
  "print rx\n"
  "print rx\n"
  "end\n"
#endif
,
  "rem ### toaster ###\n"
#if STICK_GUEST
  "dim target, secs\n"
#endif
#if MCF52221 || MCF52233 || MCF52259 || MCF5211 || PIC32
  "dim thermocouple as pin an0 for analog input\n"
  "dim relay as pin an1 for digital output\n"
#elif MCF51JM128 || MCF51QE128 || MC9S08QE128
  "dim thermocouple as pin ptb2 for analog input\n"
  "dim relay as pin ptb3 for digital output\n"
#elif MCF51CN128
  "dim thermocouple as pin pte0 for analog input\n"
  "dim relay as pin pte1 for digital output\n"
#elif MC9S12DT256 || MC9S12DP512
  "dim thermocouple as pin pad00 for analog input\n"
  "dim relay as pin pa0 for digital output\n"
#else
#error
#endif
#if STICK_GUEST
  "data 5124, 6, 7460, 9, 8940, 3, -1, -1\n"
  "configure timer 0 for 1000 ms\n"
  "on timer 0 do gosub adjust\n"
  "rem ---------------\n"
  "while target!=-1 do\n"
  "  sleep secs s\n"
  "  read target, secs\n"
  "endwhile\n"
  "let relay = 0\n"
  "end\n"
  "sub adjust\n"
  "  if thermocouple>=target then\n"
  "    let relay = 0\n"
  "  else\n"
  "    let relay = 1\n"
  "  endif\n"
  "endsub\n"
#endif
};
#endif


// *** basic0_run() ***************************************************

// this function implements the stickos command interpreter.
void
basic0_run(char *text_in)
{
    int i;
    int d;
    int h;
    int m;
    int t;
    int cmd;
    int len;
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    bool boo;
#endif
#if ZIGFLEA
    bool reset;
    bool init;
#endif
    const char *p;
#if STICK_GUEST || SODEBUG
    const char *np;
#endif
    char *text;
    int number1;
    int number2;
#if MCF52233
    int a0, a1, a2, a3;
#endif

    text = text_in;

    parse_trim(&text);

    // parse private commands
    for (cmd = 0; cmd < LENGTHOF(commands); cmd++) {
        len = strlen(commands[cmd]);
        if (! strncmp(text, commands[cmd], len)) {
            break;
        }
    }

    if (cmd != LENGTHOF(commands)) {
        text += len;
    }
    parse_trim(&text);

    number1 = 0;
    number2 = 0;

    switch (cmd) {
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
        case command_clone:
            boo = false;
            if (parse_word(&text, "run")) {
                boo = true;
            }
            if (*text) {
                goto XXX_ERROR_XXX;
            }

            basic0_help(help_about);
            printf("cloning...\n");
            clone(boo);
            break;
#endif

        case command_connect:
            if (! zb_present) {
                printf("zigflea not present\n");
#if ! STICK_GUEST
            } else if (zb_nodeid == -1) {
                printf("zigflea nodeid not set\n");
#endif
            } else {
                if (! basic_const(&text, &number1) || number1 == -1) {
                    goto XXX_ERROR_XXX;
                }
                if (*text) {
                    goto XXX_ERROR_XXX;
                }
                
                printf("press Ctrl-D to disconnect...\n");

#if ! STICK_GUEST
                assert(main_command);
                main_command = NULL;
                terminal_command_ack(false);

                terminal_rxid = number1;
                
                while (terminal_rxid != -1) {
                    basic0_poll();
                }
#endif

                printf("...disconnected\n");
            }
            break;

#if STICK_GUEST || SODEBUG
        case command_demo:
            number1 = 0;
            if (*text) {
                if (! basic_const(&text, &number1) || number1 < 0 || number1 >= LENGTHOF(demos)) {
                    goto XXX_ERROR_XXX;
                }
            }

            number2 = 0;
            if (*text) {
                if (! basic_const(&text, &number2) || ! number2) {
                    goto XXX_ERROR_XXX;
                }
                if (*text) {
                    goto XXX_ERROR_XXX;
                }
            }

            if (! number2) {
                number2 = 10;
                code_new();
            }

            i = 0;
            for (p = (char *)demos[number1]; *p; p = np+1) {
                np = strchr(p, '\n');
                assert(np);
                strncpy((char *)big_buffer, p, np-p);
                big_buffer[np-p] = '\0';
                code_insert(number2+i*10, (char *)big_buffer, 0);
                i++;
            }
            break;
#endif

        case command_help:
#if ! SODEBUG || STICK_GUEST
            if (! *text) {
                p = help_general;
            } else
#endif
            if (parse_word(&text, "about")) {
                p = help_about;
#if ! SODEBUG || STICK_GUEST
            } else if (parse_word(&text, "commands")) {
                p = help_commands;
            } else if (parse_word(&text, "modes")) {
                p = help_modes;
            } else if (parse_word(&text, "statements")) {
                p = help_statements;
            } else if (parse_word(&text, "devices")) {
                p = help_devices;
            } else if (parse_word(&text, "blocks")) {
                p = help_blocks;
            } else if (parse_word(&text, "expressions")) {
                p = help_expressions;
            } else if (parse_word(&text, "strings")) {
                p = help_strings;
            } else if (parse_word(&text, "variables")) {
                p = help_variables;
            } else if (parse_word(&text, "pins")) {
                p = help_pins;
#if MCF52259
            } else if (parse_word(&text, "board")) {
                p = help_board;
#endif
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
            } else if (parse_word(&text, "clone")) {
                p = help_clone;
#endif
            } else if (parse_word(&text, "zigflea")) {
                p = help_zigflea;
#endif
            } else {
                goto XXX_ERROR_XXX;
            }
            if (*text) {
                goto XXX_ERROR_XXX;
            }
            basic0_help((char *)p);
            break;

#if MCF52233
        case command_ipaddress:
            if (*text) {
                if (parse_word(&text, "dhcp")) {
                    a0 = a1 = a2 = a3 = 0;
                } else {
                    (void)basic_const(&text, &a0);
                    (void)parse_char(&text, '.');
                    (void)basic_const(&text, &a1);
                    (void)parse_char(&text, '.');
                    (void)basic_const(&text, &a2);
                    (void)parse_char(&text, '.');
                    boo = basic_const(&text, &a3);
                    if (! boo || (a0|a1|a2|a3)>>8) {
                        goto XXX_ERROR_XXX;
                    }
                }
                if (*text) {
                    goto XXX_ERROR_XXX;
                }
                var_set_flash(FLASH_IPADDRESS, a0<<24|a1<<16|a2<<8|a3);
            } else {
                i = var_get_flash(FLASH_IPADDRESS);
                if (! i || i == -1) {
                    printf("dhcp\n");
                } else {
                    printf("%u.%u.%u.%u\n", (int)(i>>24)&0xff, (int)(i>>16)&0xff, (int)(i>>8)&0xff, (int)i&0xff);
                }
            }
            break;
#endif

#if KBD && ! STICK_GUEST
        case command_keychars:
            if (*text) {
                memset(kbd_chars, 0, sizeof(kbd_chars));
                for (i = 0; *text && i < sizeof(kbd_chars)-1; i++) {
                    kbd_chars[i] = *text++;
                }
                for (i = 0; i < sizeof(kbd_chars)/4; i++) {
                    var_set_flash(FLASH_KBDCHARS0+i, read32(kbd_chars+i*4));
                }
            } else {
                printf("%s\n", kbd_chars);
            }
            break;
#endif

        case command_nodeid:
            if (*text) {
                if (parse_word(&text, "none")) {
                    number1 = -1;
                } else {
                    if (! basic_const(&text, &number1) || number1 == -1) {
                        goto XXX_ERROR_XXX;
                    }
                }
                if (*text) {
                    goto XXX_ERROR_XXX;
                }
                var_set_flash(FLASH_NODEID, number1);
#if ! STICK_GUEST
                zb_nodeid = number1;
#endif
            } else {
                i = var_get_flash(FLASH_NODEID);
                if (i == -1) {
                    printf("none\n");
                } else {
                    printf("%u\n", i);
                }
            }
            break;
        
        case command_reset:
            if (*text) {
                goto XXX_ERROR_XXX;
            }

#if ! STICK_GUEST
            (void)splx(7);
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
            MCF_RCM_RCR = MCF_RCM_RCR_SOFTRST;
#elif MCF51JM128 || MCF51CN128 || MCF51QE128
            asm {
                move.l  #0x00000000,d0
                movec   d0,CPUCR
                trap    #0
            };
#elif MC9S08QE128
            asm(stop);
#elif MC9S12DT256 || MC9S12DP512
            COPCTL = 0x01; // cop activated with shortest timeout 
            ARMCOP = 0x47; // here we will get kicked by the dog
#elif PIC32
            SYSKEY = 0;
            SYSKEY = 0xAA996655;
            SYSKEY = 0x556699AA;
            RSWRSTSET = _RSWRST_SWRST_MASK;
            while (RSWRST, true) {
                // NULL
            }
#else
#error
#endif
            ASSERT(0);
#endif
            break;

#if DOWNLOAD || UPGRADE
#if DOWNLOAD
        case command_download:  // relay S19 file to QSPI to EzPort
#endif
#if UPGRADE
        case command_upgrade:  // upgrade StickOS S19 file
#endif
            number1 = 0;
#if DOWNLOAD
            if (cmd == command_download) {
                // get fsys_frequency
                (void)basic_const(&text, &number1);
                if (! number1 || *text) {
                    goto XXX_ERROR_XXX;
                }
                // validate fsys_frequency
                if (number1 < 1000000 || number1 > 80000000) {
                    goto XXX_ERROR_XXX;
                }
            }
#endif
            flash_upgrade(number1);
            break;
#endif

        case command_uptime:
            if (*text) {
                goto XXX_ERROR_XXX;
            }

            t = seconds;
            d = t/(24*60*60);
            t = t%(24*60*60);
            h = t/(60*60);
            t = t%(60*60);
            m = t/(60);
            printf("%dd %dh %dm\n", d, h, m);
            break;
            
#if ZIGFLEA
        case command_zigflea:
            reset = parse_word(&text, "reset");
            init = parse_word(&text, "init");
            if (*text) {
                goto XXX_ERROR_XXX;
            }
#if ! STICK_GUEST
            zb_diag(reset, init);
#endif
            break;
#endif

        case LENGTHOF(commands):
            // this is not a private command; process a public command...
            basic_run(text_in);
            break;

        default:
            assert(0);
            break;
    }

    return;

XXX_ERROR_XXX:
    terminal_command_error(text-text_in);
}

#if ! STICK_GUEST
void
basic0_poll(void)
{
    terminal_poll();
    var_poll();

#if USB_HOST
    // flush dirty lbas to the filesystem
    flush_log_file();
#endif
}
#endif

