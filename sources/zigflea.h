// *** zigflea.h ******************************************************

enum {
    zb_class_none,
    zb_class_receive,
    zb_class_print,
    zb_class_remote_set
};

#define ZB_PAYLOAD_SIZE  114

extern int32 zb_nodeid;
extern bool zb_present;

typedef void (*zb_recv_cbfn)(int nodeid, int length, byte *buffer);

#if ! STICK_GUEST
#if ! PIC32
INTERRUPT
void
zb_isr(void);
#endif

#if MCF51JM128 || MCF51CN128 || MCF51QE128
interrupt
void
zb_pre_isr(void);
#endif
#endif

bool
zb_send(int nodeid, uint8 class, int length, const byte *buffer);

void
zb_poll(void);

void
zb_drop(bool drop);

#if ZIGFLEA
void
zb_diag(bool reset, bool init);
#endif

void
zb_register(uint8 class, zb_recv_cbfn cbfn);

void
zb_initialize(void);

