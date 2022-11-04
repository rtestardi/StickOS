// *** util.h *********************************************************

#if MCF52221 || MCF52233 || MCF52259 || MCF5211
#define SPL_PIT0  6  // pit0 isr runs at interrupt level 6
#define SPL_USB  4  // usb isr runs at interrupt level 4
#define SPL_IRQ4  4  // irq4 isr runs at interrupt level 4 (fixed, zigflea)
#define SPL_SERIAL  4  // uart0 isr runs at interrupt level 4
#define SPL_IRQ1  1  // irq1 isr runs at interrupt level 1 (fixed, sleep or zigflea)
#elif MCF51JM128 || MCF51CN128 || MCF51QE128 || MC9S08QE128 || MC9S12DT256 || MC9S12DP512
#define SPL_PIT0  6  // pit0 isr runs at interrupt level 6
#define SPL_USB  6  // usb isr runs at interrupt level 6
#define SPL_IRQ4  4  // irq isr runs at interrupt level 4 (zigflea)
#define SPL_SERIAL  3  // uart0 isr runs at interrupt level 3
#elif PIC32
#define SPL_PIT0  6  // pit0 isr runs at interrupt level 6
#define SPL_USB  6  // usb isr runs at interrupt level 6
#define SPL_IRQ4  4  // irq isr runs at interrupt level 4 (zigflea)
#define SPL_SERIAL  4
#else
#error
#endif

// REVISIT -- align all bytecode values and eliminate these (except for qspi/i2c)?
#if PIC32 && ! _WIN32
void
write32(byte *addr, uint32 data);

uint32
read32(const byte *addr);

void
write16(byte *addr, uint16 data);

uint16
read16(const byte *addr);
#else
// N.B. believe it or not, #defining these makes a 10% difference in
//      ColdFire performance!
#define write32(a, d)  (*(uint32 *)(a) = (d))
#define read32(a)  (*(uint32 *)(a))
#define write16(a, d)  (*(uint16 *)(a) = (d))
#define read16(a)  (*(uint16 *)(a))
#endif

uint32 read_n_bytes(int size, const volatile void *addr);
void write_n_bytes(int size, volatile void *addr, uint32 value);

// N.B. the usb controller bdt data structures and the usb protocol
// layers are defined to be little endian and the coldfire core is
// big endian, so we have to byteswap.  the zigflea transceiver and
// protocol layers are defined to be big endian and the pic32 core
// is little endian, so we have to byteswap.

#if PIC32 || _WIN32
#define TF_BIG(x)  byteswap((x), sizeof(x))
#define TF_LITTLE(x)  (x)
#else
#define TF_BIG(x)  (x)
#define TF_LITTLE(x)  byteswap((x), sizeof(x))
#endif

uint32
byteswap(uint32 x, uint32 size);

#if MC9S08QE128 || MC9S12DT256 || MC9S12DP512
#pragma CODE_SEG __NEAR_SEG NON_BANKED
#endif
// return the current interrupt mask level
int
gpl(void);

// set the current interrupt mask level and return the old one
int
splx(int level);

// delay for the specified number of milliseconds
void
delay(int32 ms);
#if MC9S08QE128 || MC9S12DT256 || MC9S12DP512
#pragma CODE_SEG DEFAULT
#endif

int
gethex(char *p);

int
get2hex(char **p);

void
tailtrim(char *p);

#if MC9S08QE128 || MC9S12DT256 || MC9S12DP512
#pragma CODE_SEG __NEAR_SEG NON_BANKED
#endif
void *
memcpy(void *d,  const void *s, size_t n);
#if MC9S08QE128 || MC9S12DT256 || MC9S12DP512
#pragma CODE_SEG DEFAULT
#endif

void *
memmove(void *d,  const void *s, size_t n);

void *
memset(void *p,  int d, size_t n);

int
memcmp(const void *d,  const void *s, size_t n);

size_t
strlen(const char *s);

char *
strcat(char *dest, const char *src);

char *
strncat(char *dest, const char *src, size_t n);

char *
strcpy(char *dest, const char *src);

char *
strncpy(char *dest, const char *src, size_t n);

int
strcmp(const char *s1, const char *s2);

int
strncmp(const char *s1, const char *s2, size_t n);

char *
strstr(const char *s1, const char *s2);

#if MC9S08QE128 || MC9S12DT256 || MC9S12DP512
#pragma CODE_SEG __NEAR_SEG NON_BANKED
#endif
char *
strchr(const char *s, int c);
#if MC9S08QE128 || MC9S12DT256 || MC9S12DP512
#pragma CODE_SEG DEFAULT
#endif

int
isdigit(int c);

int
isspace(int c);

int
isupper(int c);

int
islower(int c);

int
isalpha(int c);

int
isalnum(int c);

int
isprint(int c);

uint16
get_sr(void);

void
set_sr(uint16 csr);

uint32
random_32(
    void
    );
