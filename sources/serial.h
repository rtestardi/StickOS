// *** serial.h ************************************************************

#if PIC32 || MCF51CN128
#if CHIPKIT
#define SERIAL_UART  0
#else
#define SERIAL_UART  1
#endif
#else
#define SERIAL_UART  0
#endif

extern bool serial_active;

extern int serial_baudrate;

void
serial_disable(void);

void
serial_command_ack(void);

#if ! PIC32
INTERRUPT
void
serial_isr(void);
#endif

void
serial_send(const byte *buffer, int length);

void
serial_initialize(void);

