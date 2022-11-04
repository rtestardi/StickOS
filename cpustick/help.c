// GENERATE_HELP_BEGIN

#if ! SODEBUG || STICK_GUEST
static char *const help_general =
"fo\262mor\245information:\n"
"\300hel\260about\n"
"\300hel\260commands\n"
"\300hel\260modes\n"
"\300hel\260statements\n"
"\300hel\260blocks\n"
"\300hel\260devices\n"
"\300hel\260expressions\n"
"\300hel\260strings\n"
"\300hel\260variables\n"
"\300hel\260pins\n"
#if MCF52259
"\300hel\260board\n"
#endif
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
"\300hel\260clone\n"
#endif
"\300hel\260zigflea\n"
"\n"
"se\245also:\n"
"\300http://www.cpustick.com\n"
;

static char * const help_commands =
"<Ctrl-C>\324-\015sto\260program\n"
"aut\257<line>\321-\015automaticall\271numbe\262progra\255lines\n"
"clea\262[flash]\317-\015clea\262ra\255[an\244flash\235variables\n"
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
"clon\245[run]\321-\015clon\245flas\250t\257slav\245MC\225[an\244run]\n"
#endif
"cls\331-\015clea\262termina\254screen\n"
"con\264[<line>]\317-\015continu\245progra\255fro\255stop\n"
"delet\245([<line>][-][<line>]|<subname>\011-\015delet\245progra\255lines\n"
#if DOWNLOAD
"downloa\244<slav\245Hz>\311-\015downloa\244flas\250t\257slav\245MCU\n"
#endif
"dir\331-\015lis\264save\244programs\n"
"edi\264<line>\321-\015edi\264progra\255line\n"
"hel\260[<topic>]\316-\015onlin\245help\n"
"lis\264([<line>][-][<line>]|<subname>\011-\015lis\264progra\255lines\n"
"loa\244<name>\321-\015loa\244save\244program\n"
"memory\326-\015prin\264memor\271usage\n"
"new\331-\015eras\245cod\245ra\255an\244flas\250memories\n"
"profil\245([<line>][-][<line>]|<subname>\011-\015displa\271profil\245info\n"
"purg\245<name>\320-\015purg\245save\244program\n"
"renumbe\262[<line>]\313-\015renumbe\262progra\255line\263(an\244save)\n"
"reset\327-\015rese\264th\245MCU!\n"
"ru\256[<line>]\320-\015ru\256program\n"
"sav\245[<name>|library]\307-\015sav\245cod\245ra\255t\257flas\250memory\n"
"subs\330-\015lis\264su\242names\n"
"undo\330-\015und\257cod\245change\263sinc\245las\264save\n"
#if UPGRADE
"upgrade\325-\015upgrad\245StickO\223firmware!\n"
#endif
"uptime\326-\015prin\264tim\245sinc\245las\264reset\n"
"\n"
"fo\262mor\245information:\n"
"\300hel\260modes\n"
;

static char * const help_modes =
"analo\247[<millivolts>]\313-\015set/displa\271analo\247voltag\245scale\n"
"bau\244[<rate>]\323-\015set/displa\271uar\264consol\245bau\244rate\n"
"autoru\256[on|off]\320-\015autoru\256mod\245(o\256reset)\n"
"ech\257[on|off]\323-\015termina\254ech\257mode\n"
"inden\264[on|off]\321-\015listin\247inden\264mode\n"
#if MCF52233
"ipaddres\263[dhcp|<ipaddress>]\304-\015set/displa\271i\260address\n"
#endif
#if KBD
"keychar\263[<keychars>]\313-\015set/displa\271keypa\244sca\256chars\n"
#endif
"nodei\244[<nodeid>|none]\312-\015set/displa\271zigfle\241nodeid\n"
"number\263[on|off]\320-\015listin\247lin\245number\263mode\n"
"pin\263[<assign\036[<pinname>|none]]\300-\015set/displa\271StickO\223pi\256assignments\n"
"promp\264[on|off]\321-\015termina\254promp\264mode\n"
"serv\257[<Hz>]\324-\015set/displa\271serv\257H\272(o\256reset)\n"
"ste\260[on|off]\323-\015debugge\262single-ste\260mode\n"
"trac\245[on|off]\322-\015debugge\262trac\245mode\n"
#if USB_HOST
"usbhos\264[on|off]\320-\015US\202hos\264mod\245(o\256reset)\n"
#endif
"watchsmar\264[on|off]\315-\015low-overhea\244watchpoin\264mode\n"
"\n"
"pi\256assignments:\n"
"\300heartbeat\300safemode*\n"
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
"\300qspi_cs*\300clone_rst*\300zigflea_rst*\300zigflea_attn*\300zigflea_rxtxen\n"
#else
"\300qspi_cs*\300zigflea_rst*\300zigflea_attn*\300zigflea_rxtxen\n"
#endif
"\n"
"fo\262mor\245information:\n"
"\300hel\260pins\n"
;

static char * const help_statements =
"<line>\337-\015delet\245progra\255lin\245fro\255cod\245ram\n"
"<line\036<statement>\300/\017comment\307-\015ente\262progra\255lin\245int\257cod\245ram\n"
"\n"
"<variable>[$\235\035<expression\036[\014...]\301-\015assig\256variable\n"
"\037[dec|hex|raw\235<expression\036[\014...\235[;\235-\015prin\264results\n"
"asser\264<expression>\322-\015brea\253i\246expressio\256i\263false\n"
"dat\241<n\036[\014...]\325-\015read-onl\271data\n"
"di\255<variable>[$][[n]\235[a\263...\235[\014...\235-\015dimensio\256variables\n"
"end\342-\015en\244program\n"
#if BADGE_BOARD
"jm(clear|set\011<r>\014<c>\317-\015clear/se\264row/colum\256o\246LE\204matrix\n"
"jmscrol\254...\331-\015scrol\254printou\264t\257LE\204matrix\n"
#endif
"halt\341-\015loo\260forever\n"
"inpu\264[dec|hex|raw\235<variable>[$\235[\014...\235-\015inpu\264data\n"
"labe\254<label>\330-\015read/dat\241label\n"
#if LCD
"lc\244<pos>\014[dec|hex|raw\235<expression\036[\014...\235[;\235-\015displa\271result\263o\256lcd\n"
#endif
"le\264<variable>[$\235\035<expression\036[\014...\235-\015assig\256variable\n"
"prin\264[dec|hex|raw\235<expression\036[\014...\235[;\235-\015prin\264results\n"
"rea\244<variable\036[\014...]\316-\015rea\244read-onl\271dat\241int\257variables\n"
"re\255<remark>\331-\015remark\n"
"restor\245[<label>]\324-\015restor\245read-onl\271dat\241pointer\n"
"slee\260<expression\036(s|ms|us)\311-\015dela\271progra\255execution\n"
"stop\341-\015inser\264breakpoin\264i\256code\n"
"vprin\264<variable>[$\235\035[dec|hex|raw\235<expression\036[\014...\235-\015prin\264t\257variable\n"
"\n"
"fo\262mor\245information:\n"
"\300hel\260blocks\n"
"\300hel\260devices\n"
"\300hel\260expressions\n"
"\300hel\260strings\n"
"\300hel\260variables\n"
;

static char * const help_blocks =
"i\246<expression\036then\n"
"[elsei\246<expression\036then]\n"
"[else]\n"
"endif\n"
"\n"
"fo\262<variable\036\035<expression\036t\257<expression\036[ste\260<expression>]\n"
"\300[(break|continue\011[n]]\n"
"next\n"
"\n"
"whil\245<expression\036do\n"
"\300[(break|continue\011[n]]\n"
"endwhile\n"
"\n"
"do\n"
"\300[(break|continue\011[n]]\n"
"unti\254<expression>\n"
"\n"
"gosu\242<subname\036[<expression>\014...]\n"
"\n"
"su\242<subname\036[<param>\014...]\n"
"\300[return]\n"
"endsub\n"
;

static char * const help_devices =
"timers:\n"
"\300configur\245time\262<n\036fo\262<n\036(s|ms|us)\n"
"\300o\256time\262<n\036d\257<statement>\316-\015o\256time\262execut\245statement\n"
"\300of\246time\262<n>\334-\015disabl\245time\262interrupt\n"
"\300mas\253time\262<n>\333-\015mask/hol\244time\262interrupt\n"
"\300unmas\253time\262<n>\331-\015unmas\253time\262interrupt\n"
"\n"
"uarts:\n"
"\300configur\245uar\264<n\036fo\262<n\036bau\244<n\036dat\241(even|odd|no\011parit\271[loopback]\n"
"\300o\256uar\264<n\036(input|output\011d\257<statement>\300-\015o\256uar\264execut\245statement\n"
"\300of\246uar\264<n\036(input|output)\316-\015disabl\245uar\264interrupt\n"
"\300mas\253uar\264<n\036(input|output)\315-\015mask/hol\244uar\264interrupt\n"
"\300unmas\253uar\264<n\036(input|output)\313-\015unmas\253uar\264interrupt\n"
"\300uar\264<n\036(read|write\011<variable\036[\014...]\301-\015perfor\255uar\264I/O\n"
"\n"
"i2c:\n"
"\300i2\243(star\264<addr>|(read|write\011<variable\036[\014...]|stop\011-\015maste\262i2\243I/O\n"
"\n"
"qspi:\n"
"\300qsp\251<variable\036[\014...]\322-\015maste\262qsp\251I/O\n"
"\n"
"watchpoints:\n"
"\300o\256<expression\036d\257<statement>\313-\015o\256exp\262execut\245statement\n"
"\300of\246<expression>\331-\015disabl\245exp\262watchpoint\n"
"\300mas\253<expression>\330-\015mask/hol\244exp\262watchpoint\n"
"\300unmas\253<expression>\326-\015unmas\253exp\262watchpoint\n"
;

static char * const help_expressions =
"th\245followin\247operator\263ar\245supporte\244a\263i\256C,\n"
"i\256orde\262o\246decreasin\247precedence:\n"
"\300<n>\325-\015decima\254constant\n"
"\3000x<n>\323-\015hexadecima\254constant\n"
"\300'c'\325-\015characte\262constant\n"
"\300<variable>\316-\015simpl\245variable\n"
"\300<variable>[<expression>]\300-\015arra\271variabl\245element\n"
"\300<variable>#\315-\015lengt\250o\246arra\271o\262string\n"
"\300(\301)\323-\015grouping\n"
"\300!\301~\323-\015logica\254not\014bitwis\245not\n"
"\300*\301/\301%\317-\015times\014divide\014mod\n"
"\300+\301-\323-\015plus\014minus\n"
"\300>>\300<<\322-\015shif\264right\014left\n"
"\300<=\300<\300>=\300>\314-\015inequalities\n"
"\300==\300!=\322-\015equal\014no\264equal\n"
"\300|\301^\301&\317-\015bitwis\245or\014xor\014and\n"
"\300||\300^^\300&&\316-\015logica\254or\014xor\014and\n"
"fo\262mor\245information:\n"
"\300hel\260variables\n"
;

static char * const help_strings =
"v\004i\263\241nul-terminate\244vie\267int\257\241byt\245arra\271v[]\n"
"\n"
"strin\247statements:\n"
"\300dim\014input\014let\014print\014vprint\n"
"\300i\246<expression\036<relation\036<expression\036then\n"
"\300whil\245<expression\036<relation\036<expression\036do\n"
"\300unti\254<expression\036<relation\036<expression\036do\n"
"\n"
"strin\247expressions:\n"
"\300\"literal\"\324-\015litera\254string\n"
"\300<variable>$\322-\015variabl\245string\n"
"\300<variable>$[<start>:<length>]\300-\015variabl\245substring\n"
"\300+\334-\015concatenate\263strings\n"
"\n"
"strin\247relations:\n"
"\300<=\300<\300>=\300>\321-\015inequalities\n"
"\300==\300!=\327-\015equal\014no\264equal\n"
"\300~\300!~\330-\015contains\014doe\263no\264contain\n"
"fo\262mor\245information:\n"
"\300hel\260variables\n"
;

static char *const help_variables =
"al\254variable\263mus\264b\245dimensioned!\n"
"variable\263dimensione\244i\256\241su\242ar\245loca\254t\257tha\264sub\n"
"simpl\245variable\263ar\245passe\244t\257su\242param\263b\271reference\033otherwise\014b\271value\n"
"arra\271variabl\245indice\263star\264a\2640\n"
"\266i\263th\245sam\245a\263v[0]\014excep\264fo\262input/print/i2c/qspi/uar\264statements\n"
"\n"
"ra\255variables:\n"
"\300di\255<var>[$][[n]]\n"
"\300di\255<var>[[n]\235a\263(byte|short)\n"
"\n"
"absolut\245variables:\n"
"\300di\255<var>[[n]\235[a\263(byte|short)\235a\264addres\263<addr>\n"
"\n"
"flas\250paramete\262variables:\n"
"\300di\255<varflash>[[n]\235a\263flash\n"
"\n"
"pi\256alia\263variables:\n"
"\300di\255<varpin\036a\263pi\256<pinname\036fo\262(digital|analog|servo|frequency|uart\011\\\n"
"\344(input|output\011\\\n"
"\344[debounced\235[inverted\235[open_drain]\n"
"\n"
"syste\255variable\263(read-only):\n"
"\300analog\300getchar"
#if KBD
"\300keychar"
#endif
"\300msecs\300nodeid\n"
"\300random\300seconds\300ticks\300ticks_per_msec\n"
"\n"
"fo\262mor\245information:\n"
"\300hel\260pins\n"
;

static char *const help_pins =
"pi\256names:\n"
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
"\3020\3071\3072\3073\3064\3035\3066\3067\n"
"\300--------\300--------\015--------\015-------\015----\015-------\015-------\015------+\n"
"\300an0\305an1\305an2\305an3\304an4\301an5\304an6\304an7\301\274AN\n"
"\300scl\305sda\366\274AS\n"
#if MCF52233 || MCF52259 || MCF5211
"\300gpt0\304gpt1\304gpt2\304gpt3\341\274TA\n"
#endif
#if MCF52259
"\312irq1*\315irq3*\310irq5*\313irq7\177\274NQ\n"
#else
"\312irq1*\326irq4*\321irq7\177\274NQ\n"
#endif
#if MCF52233
"\336irq11*\337\274GP\n"
#endif
"\300qspi_dou\264qspi_din\300qspi_clk\300qspi_cs0\305qspi_cs\022qspi_cs3\305\274QS\n"
"\300dtin0\303dtin1\303dtin2\303dtin3\340\274TC\n"
"\300utxd0\303urxd0\303urts0*\302ucts0*\337\274UA\n"
"\300utxd1\303urxd1\303urts1*\302ucts1*\337\274UB\n"
#if MCF52259
"\300fec_col\301fec_crs\301fec_rxcl\253fec_rxd[0-3]\320fec_rxd\266\274TI\n"
"\300fec_rxer\300fec_txcl\253fec_txd[0-3]\321fec_txe\256fec_txe\262\274TJ\n"
"\300utxd2\303urxd2\303urts2*\302ucts2*\337\274UC\n"
#endif
"\n"
"al\254pin\263suppor\264genera\254purpos\245digita\254input/output\n"
"an\037\035potentia\254analo\247inpu\264pin\263(mV)\n"
#if MCF52233 || MCF52259 || MCF5211
"dtin?\014gpt\037\035potentia\254analo\247outpu\264(PWM\011pin\263(mV)\n"
"dtin?\014gpt\037\035potentia\254serv\257outpu\264(PWM\011pin\263(us)\n"
#else
"dtin\037\035potentia\254analo\247outpu\264(PWM\011pin\263(mV)\n"
"dtin\037\035potentia\254serv\257outpu\264(PWM\011pin\263(us)\n"
#endif
"dtin\037\035potentia\254frequenc\271outpu\264pin\263(Hz)\n"
"urxd\037\035potentia\254uar\264inpu\264pin\263(receive\244byte)\n"
"utxd\037\035potentia\254uar\264outpu\264pin\263(transmi\264byte)\n"
#elif MCF51JM128
#if FB32
"\300a0-a7\0140-13\n"
"\300pta[0-5]\014ptc[0-1,3-6]\014ptd[0,3-7]\014pte7\014ptf[5,7]\014ptg[0-2]\n"
"\n"
"peripherals:\n"
"\300switch\317ptg0\n"
"\300buzzer\317ptf5\n"
"\300rg\242anod\245(+)\310ptg1\n"
"\300rg\242re\244cathod\245(-)\302\025(pte3)\n"
"\300rg\242gree\256cathod\245(-)\300\023(pte2)\n"
"\300rg\242blu\245cathod\245(-)\301\031(ptf1)\n"
"\300eepro\255writ\245protect\301ptg2\n"
"\n"
"jumpers:\n"
"\300d6=pwm\014d10=pwm\n"
"\n"
"al\254pin\263suppor\264genera\254purpos\245digita\254input/output\n"
"a\037\035potentia\254analo\247inpu\264pin\263(mV)\n"
"3,5,6,8,9,10,1\021\035potentia\254analo\247outpu\264(PWM\011pin\263(mV)\n"
"3,5,6,8,9,10,1\021\035potentia\254serv\257outpu\264(PWM\011pin\263(us)\n"
"3,5,6,8,9,10,1\021\035potentia\254frequenc\271outpu\264pin\263(Hz)\n"
#else
"\3020\3051\3052\3053\3054\3055\3056\3057\n"
"\300------\015------\015------\015------\015------\015------\015------\015--------+\n"
"\300pta0\302pta1\302pta2\302pta3\302pta4\302pta5\322\274POR\224A\n"
"\300ptb0\302ptb1\302ptb2\302ptb3\302ptb4\302ptb5\302ptb6\302ptb7\302\274POR\224B\n"
"\300ptc0\302ptc1\302ptc2\302ptc3\302ptc4\302ptc5\302ptc6\312\274POR\224C\n"
"\300ptd0\302ptd1\302ptd2\302ptd3\302ptd4\302ptd5\302ptd6\302ptd7\302\274POR\224D\n"
"\300pte0\302pte1\302pte2\302pte3\302pte4\302pte5\302pte6\302pte7\302\274POR\224E\n"
"\300ptf0\302ptf1\302ptf2\302ptf3\302ptf4\302ptf5\302ptf6\302ptf7\302\274POR\224F\n"
"\300ptg0\302ptg1\302ptg2\302ptg3\342\274POR\224G\n"
"\n"
"al\254pin\263suppor\264genera\254purpos\245digita\254input/output\n"
"ptb?\014ptd[0134\235\035potentia\254analo\247inpu\264pin\263(mV)\n"
"pte[23]\014ptf[0-5\235\035potentia\254analo\247outpu\264(PWM\011pin\263(mV)\n"
"pte[23]\014ptf[0-5\235\035potentia\254serv\257outpu\264(PWM\011pin\263(us)\n"
"pte[23]\014ptf[0-5\235\035potentia\254frequenc\271outpu\264pin\263(Hz)\n"
"pte\021(u1)\014ptc\025(u2\011\035potentia\254uar\264inpu\264pin\263(receive\244byte)\n"
"pte\020(u1)\014ptc\023(u2\011\035potentia\254uar\264outpu\264pin\263(transmi\264byte)\n"
#endif
#elif MCF51CN128
"\3020\3051\3052\3053\3054\3055\3056\3057\n"
"\300------\015------\015------\015------\015------\015------\015------\015--------+\n"
"\300pta0\302pta1\302pta2\302pta3\302pta4\302pta5\302pta6\302pta7\302\274POR\224A\n"
"\300ptb0\302ptb1\302ptb2\302ptb3\302ptb4\302ptb5\302ptb6\302ptb7\302\274POR\224B\n"
"\300ptc0\302ptc1\302ptc2\312ptc4\302ptc5\302ptc6\302ptc7\302\274POR\224C\n"
"\300ptd0\302ptd1\302ptd2\302ptd3\322ptd6\302ptd7\302\274POR\224D\n"
"\300pte0\302pte1\302pte2\302pte3\302pte4\302pte5\302pte6\302pte7\302\274POR\224E\n"
"\300ptf0\302ptf1\302ptf2\302ptf3\302ptf4\302ptf5\302ptf6\302ptf7\302\274POR\224F\n"
"\300ptg0\302ptg1\302ptg2\302ptg3\302ptg4\302ptg5\302ptg6\302ptg7\302\274POR\224G\n"
"\300pth0\302pth1\302pth2\302pth3\302pth4\302pth5\302pth6\302pth7\302\274POR\224H\n"
"\300ptj0\302ptj1\302ptj2\302ptj3\302ptj4\302ptj5\322\274POR\224J\n"
"\n"
"al\254pin\263suppor\264genera\254purpos\245digita\254input/output\n"
"ptc[4-7]\014ptd[0-37]\014pte[0-2\235\035potentia\254analo\247inpu\264pin\263(mV)\n"
"ptb[67]\014ptc0\014pte[3-5\235\035potentia\254analo\247outpu\264(PWM\011pin\263(mV)\n"
"ptb[67]\014ptc0\014pte[3-5\235\035potentia\254serv\257outpu\264(PWM\011pin\263(us)\n"
"ptb[67]\014ptc0\014pte[3-5\235\035potentia\254frequenc\271outpu\264pin\263(Hz)\n"
"ptd\021(u1)\014ptd\023(u2)\014pta\024(u3\011\035potentia\254uar\264inpu\264pin\263(receive\244byte)\n"
"ptd\020(u1)\014ptd\022(u2)\014pta\023(u3\011\035potentia\254uar\264outpu\264pin\263(transmi\264byte)\n"
#elif MCF51QE128 || MC9S08QE128
"\3020\3051\3052\3053\3054\3055\3056\3057\n"
"\300------\015------\015------\015------\015------\015------\015------\015--------+\n"
"\300pta0\302pta1\302pta2\302pta3\302pta4\302pta5\302pta6\302pta7\302\274POR\224A\n"
"\300ptb0\302ptb1\302ptb2\302ptb3\302ptb4\302ptb5\302ptb6\302ptb7\302\274POR\224B\n"
"\300ptc0\302ptc1\302ptc2\302ptc3\302ptc4\302ptc5\302ptc6\302ptc7\302\274POR\224C\n"
"\300ptd0\302ptd1\302ptd2\302ptd3\302ptd4\302ptd5\302ptd6\302ptd7\302\274POR\224D\n"
"\300pte0\302pte1\302pte2\302pte3\302pte4\302pte5\302pte6\302pte7\302\274POR\224E\n"
"\300ptf0\302ptf1\302ptf2\302ptf3\302ptf4\302ptf5\302ptf6\302ptf7\302\274POR\224F\n"
"\300ptg0\302ptg1\302ptg2\302ptg3\342\274POR\224G\n"
"\n"
"al\254pin\263suppor\264genera\254purpos\245digita\254input/output\n"
"pta[0-367]\014ptb[0-3]\014ptf[0-7]\014ptg[23\235\035potentia\254analo\247inpu\264pin\263(mV)\n"
"pta[0167]\014ptb[45]\014ptc[0-5\235\035potentia\254analo\247outpu\264(PWM\011pin\263(mV)\n"
"pta[0167]\014ptb[45]\014ptc[0-5\235\035potentia\254serv\257outpu\264(PWM\011pin\263(us)\n"
"pta[0167]\014ptb[45]\014ptc[0-5\235\035potentia\254frequenc\271outpu\264pin\263(Hz)\n"
"ptb\020(u1)\014ptc\026(u2\011\035potentia\254uar\264inpu\264pin\263(receive\244byte)\n"
"ptb\021(u1)\014ptc\027(u2\011\035potentia\254uar\264outpu\264pin\263(transmi\264byte)\n"
#elif MC9S12DT256 || MC9S12DP512
"\3020\3051\3052\3053\3054\3055\3056\3057\n"
"\300------\015------\015------\015------\015------\015------\015------\015--------+\n"
"\300pad00\301pad01\301pad02\301pad03\301pad04\301pad05\301pad06\301pad07\301\274POR\224AD0\n"
"\300pad08\301pad09\301pad10\301pad11\301pad12\301pad13\301pad14\301pad15\301\274POR\224AD1\n"
"\300pa0\303pa1\303pa2\303pa3\303pa4\303pa5\303pa6\303pa7\303\274POR\224A\n"
"\300pb0\303pb1\303pb2\303pb3\303pb4\303pb5\303pb6\303pb7\303\274POR\224B\n"
"\300pe0\303pe1\303pe2\303pe3\303pe4\303pe5\303pe6\303pe7\303\274POR\224E\n"
"\300ph0\303ph1\303ph2\303ph3\303ph4\303ph5\303ph6\303ph7\303\274POR\224H\n"
"\300pj0\303pj1\343pj6\303pj7\303\274POR\224J\n"
"\300pk0\303pk1\303pk2\303pk3\303pk4\303pk5\303pk6\303pk7\303\274POR\224K\n"
"\300pm0\303pm1\303pm2\303pm3\303pm4\303pm5\303pm6\303pm7\303\274POR\224M\n"
"\300pp0\303pp1\303pp2\303pp3\303pp4\303pp5\303pp6\303pp7\303\274POR\224P\n"
"\300ps0\303ps1\303ps2\303ps3\303ps4\303ps5\303ps6\303ps7\303\274POR\224S\n"
"\300pt0\303pt1\303pt2\303pt3\303pt4\303pt5\303pt6\303pt7\303\274POR\224T\n"
"\n"
"al\254pin\263suppor\264genera\254purpos\245digita\254input\n"
"al\254pin\263excep\264pad?\037an\244pe[01\235suppor\264genera\254purpos\245digita\254output\n"
"pad?\037\035potentia\254analo\247inpu\264pin\263(mV)\n"
"pp\037\035potentia\254analo\247outpu\264(PWM\011pin\263(mV)\n"
"pp\037\035potentia\254serv\257outpu\264(PWM\011pin\263(us)\n"
"pt\037\035potentia\254frequenc\271outpu\264pin\263(Hz)\n"
"ps\020(u0)\014ps\022(u1\011\035potentia\254uar\264inpu\264pin\263(receive\244byte)\n"
"ps\021(u0)\014ps\023(u1\011\035potentia\254uar\264outpu\264pin\263(transmi\264byte)\n"
#elif PIC32
#if defined CHIPKIT
#if defined(__32MX320F128H__)
"\300a0-a11\0140-13\01426-41\01443\014rg2\014rg3\014rg9\n"
"\n"
"jumpers:\n"
"\300jp5,jp7=master\014jp4=rd4\n"
#else
"\300a0-a15\0140-13\01416-23\01428-39\01443-49\01452-53\01470-85\n"
"\n"
"pi\256aliases:\n"
"\30040=a11\01441=a13\01442=a12\01450=29\01451=43\n"
"\n"
"jumpers:\n"
"\300jp3,jp4=master\n"
#endif
"\n"
"al\254pin\263suppor\264genera\254purpos\245digita\254input/output\n"
"a\037\035potentia\254analo\247inpu\264pin\263(mV)\n"
"3,5,6,9,1\020\035potentia\254analo\247outpu\264(PWM\011pin\263(mV)\n"
"3,5,6,9,1\020\035potentia\254serv\257outpu\264(PWM\011pin\263(us)\n"
"3,5,6,9,1\020\035potentia\254frequenc\271outpu\264pin\263(Hz)\n"
#else
"\3000/8\3031/9\3032/10\3023/11\3024/12\3025/13\3026/14\3027/15\n"
"\300------\015------\015------\015------\015------\015------\015------\015--------+\n"
#if PIC32PORTA
"\300ra0\303ra1\303ra2\303ra3\303ra4\303ra5\303ra6\303ra7\303\274POR\224A\n"
"\310ra9\303ra10\332ra14\302ra15\302|\304A+8\n"
#endif
"\300an0\303an1\303an2\303an3\303an4\303an5\303an6\303an7\303\274POR\224B\n"
"\300an8\303an9\303an10\302an11\302an12\302an13\302an14\302an15\302|\304B+8\n"
"\310rc1\303rc2\303rc3\303rc4\333\274POR\224C\n"
"\340rc12\302rc13\302rc14\302rc15\302|\304C+8\n"
"\300rd0\303rd1\303rd2\303rd3\303rd4\303rd5\303rd6\303rd7\303\274POR\224D\n"
"\300rd8\303rd9\303rd10\302rd11\302rd12\302rd13\302rd14\302rd15\302|\304D+8\n"
"\300re0\303re1\303re2\303re3\303re4\303re5\303re6\303re7\303\274POR\224E\n"
"\300re8\303re9\363|\304E+8\n"
"\300rf0\303rf1\303rf2\303rf3\303rf4\303rf5\323\274POR\224F\n"
"\300rf8\333rf12\302rf13\322|\304F+8\n"
"\300rg0\303rg1\303rg2\303rg3\323rg6\303rg7\303\274POR\224G\n"
"\300rg8\303rg9\323rg12\302rg13\302rg14\302rg15\302|\304G+8\n"
"\n"
"al\254pin\263suppor\264genera\254purpos\245digita\254input/output\n"
"an\037\035potentia\254analo\247inpu\264pin\263(mV)\n"
"rd[0-4\235\035potentia\254analo\247outpu\264(PWM\011pin\263(mV)\n"
"rd[0-4\235\035potentia\254serv\257outpu\264(PWM\011pin\263(us)\n"
"rd[0-4\235\035potentia\254frequenc\271outpu\264pin\263(Hz)\n"
"rf\024(u2\011\035potentia\254uar\264inpu\264pin\263(receive\244byte)\n"
"rf\025(u2\011\035potentia\254uar\264outpu\264pin\263(transmi\264byte)\n"
#endif
#else
#error
#endif
;

#if MCF52259
static char *const help_board =
"1\3002\3003\3004\3005\3006\3007\3008\3009\3001\0201\02112\3021\3002\3003\3004\3005\3006\3007\3008\3009\3001\0201\02112\n"
"\n"
"g\300+\300q\300q\300q\300q\300r\300r\300q\300q\300r\300v\303u\300u\300u\300u\300g\300g\300g\300g\300i\300i\300i\300i\n"
"n\3003\300s\300s\300s\300s\300c\300s\300s\300s\300s\300b\303t\300r\300r\300c\300p\300p\300p\300p\300r\300r\300r\300r\n"
"d\300V\300p\300p\300p\300p\300o\300t\300p\300p\300t\300u\303x\300x\300t\300t\300t\300t\300t\300t\300q\300q\300q\300q\n"
"\304i\300i\300i\300i\300n\300i\300i\300i\300o\300s\303d\300d\300s\300s\3000\3001\3002\3003\3001\3003\3005\3007\n"
"\304_\300_\300_\300_\300*\300n\300_\300_\300u\3062\3002\3002\3002\314*\300*\300*\300*\n"
"\304c\300d\300d\300c\303*\300c\300c\300t\314*\300*\326\n"
"\304l\300o\300i\300s\306s\300s\300*\350\n"
"\304k\300u\300n\3000\3062\3003\353\n"
"\307t\375\n"
"\n"
"antenna\332MCU\336USB\n"
"\n"
"\n"
"\304u\300u\306u\300u\361\n"
"u\300u\300r\300c\300u\300u\300r\300c\300d\300d\300d\300d\345\n"
"t\300r\300t\300t\300t\300r\300t\300t\300t\300t\300t\300t\345\n"
"x\300x\300s\300s\300x\300x\300s\300s\300i\300i\300i\300i\303g\300+\300s\300s\300a\300a\300a\300a\300a\300a\300a\300a\n"
"d\300d\3000\3000\300d\300d\3001\3001\300n\300n\300n\300n\303n\3003\300c\300d\300n\300n\300n\300n\300n\300n\300n\300n\n"
"0\3000\300*\300*\3001\3001\300*\300*\3000\3001\3002\3003\303d\300V\300l\300a\3000\3001\3002\3003\3004\3005\3006\3007\n"
"\n"
"1\3002\3003\3004\3005\3006\3007\3008\3009\3001\0201\02112\3021\3002\3003\3004\3005\3006\3007\3008\3009\3001\0201\02112\n"
;
#endif

#if MCF52221 || MCF52233 || MCF52259 || MCF5211
static char *const help_clone =
"clon\245cable:\n"
"\300master\311slave\n"
"\300---------\306----------------\n"
"\300qspi_clk\307qspi_cl\253(ezpck)\n"
"\300qspi_din\307qspi_dou\264(ezpq)\n"
"\300qspi_dout\306qspi_di\256(ezpd)\n"
"\300qspi_cs0\307rcon\177(ezpcs*)\n"
"\300pin\263clone_rst*\300rsti*\n"
"\300vss\314vss\n"
"\300vdd\314vdd\n"
;
#endif

static char *const help_zigflea =
"connec\264<nodeid>\314-\015connec\264t\257MC\225<nodeid\036vi\241zigflea\n"
"<Ctrl-D>\324-\015disconnec\264fro\255zigflea\n"
"\n"
"remot\245nod\245variables:\n"
"\300di\255<varremote>[[n]\235a\263remot\245o\256nodei\244<nodeid>\n"
"\n"
"zigfle\241cable:\n"
"\300MCU\320MC1320X\n"
"\300-------------\306-----------\n"
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
"\300qspi_clk\313spiclk\n"
"\300qspi_din\313miso\n"
"\300qspi_dout\312mosi\n"
#if MCF52259
"\300irq1*\316irq*\n"
#else
"\300irq4*\316irq*\n"
#endif
#elif MCF51JM128
"\300spsck\021(pte6)\306spiclk\n"
"\300miso\021(pte4)\307miso\n"
"\300mosi\021(pte5)\307mosi\n"
"\300irq*\317irq*\n"
#elif MCF51CN128
"\300spsck\021(ptb5)\306spiclk\n"
"\300miso\021(ptb4)\307miso\n"
"\300mosi\021(ptb3)\307mosi\n"
"\300irq\177(ptc4)\310irq*\n"
#elif MCF51QE128 || MC9S08QE128
"\300spsck\021(ptb2)\306spiclk\n"
"\300miso\021(ptb4)\307miso\n"
"\300mosi\021(ptb3)\307mosi\n"
"\300irq*\317irq*\n"
#elif MC9S12DT256 || MC9S12DP512
"\300sck\020(pm5)\311spiclk\n"
"\300miso\020(pm2)\310miso\n"
"\300mosi\020(pm4)\310mosi\n"
"\300irq\177(pe1)\311irq*\n"
#elif PIC32
// REVISIT -- implement zigflea on MRF24J40
"\300sck1\317spiclk\n"
"\300sdi1\317miso\n"
"\300sdo1\317mosi\n"
"\300int1\317irq*\n"
#else
#error
#endif
"\300pin\263qspi_cs*\306ce*\n"
"\300pin\263zigflea_rst*\302rst*\n"
"\300pin\263zigflea_rxtxen\300rxtxen\n"
"\300vss\320vss\n"
"\300vdd\320vdd\n"
;
#endif

