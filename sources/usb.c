// *** usb.c **********************************************************
// this file implements a generic usb device driver; the FTDI transport
// sits on top of this module to implement a specific usb device.

#include "main.h"
#if USBOTG

#if PIC32
// REVISIT -- move to relocated compat.h
#define MCF_USB_OTG_CTL  U1CON
#define MCF_USB_OTG_CTL_USB_EN_SOF_EN  _U1CON_SOFEN_MASK
#define MCF_USB_OTG_OTG_CTRL  U1OTGCON
#define MCF_USB_OTG_OTG_CTRL_DP_HIGH  _U1OTGCON_DPPULUP_MASK
#define MCF_USB_OTG_OTG_CTRL_OTG_EN  _U1OTGCON_OTGEN_MASK
#define MCF_USB_OTG_INT_STAT  U1IR
#define MCF_USB_OTG_INT_ENB  U1IE
#define MCF_USB_OTG_INT_ENB_USB_RST_EN  _U1IE_URSTIE_MASK
#define MCF_USB_OTG_ADDR  U1ADDR
#define MCF_USB_OTG_CTL_ODD_RST  _U1CON_PPBRST_MASK
#define MCF_USB_OTG_INT_ENB_SLEEP_EN  _U1IE_IDLEIE_MASK
#define MCF_USB_OTG_INT_ENB_TOK_DNE_EN  _U1IE_TRNIE_MASK
#define MCF_USB_OTG_ENDPT_EP_HSHK  _U1EP0_EPHSHK_MASK
#define MCF_USB_OTG_ENDPT_EP_TX_EN  _U1EP0_EPTXEN_MASK
#define MCF_USB_OTG_ENDPT_EP_RX_EN  _U1EP0_EPRXEN_MASK
#define MCF_USB_OTG_ENDPT_EP_CTL_DIS  _U1EP0_EPCONDIS_MASK
#define MCF_USB_OTG_INT_STAT_TOK_DNE  _U1IR_TRNIF_MASK
#define MCF_USB_OTG_STAT  U1STAT
#define MCF_USB_OTG_STAT_TX  _U1STAT_DIR_MASK
#define MCF_USB_OTG_STAT_ODD  _U1STAT_PPBI_MASK
#define MCF_USB_OTG_CTL_TXSUSPEND_TOKENBUSY  _U1CON_TOKBUSY_MASK
#define MCF_USB_OTG_INT_STAT_USB_RST  _U1IR_URSTIF_MASK
#define MCF_USB_OTG_INT_STAT_SLEEP  _U1IR_IDLEIF_MASK
#define MCF_USB_OTG_SOF_THLD  U1SOF
#define MCF_USB_OTG_BDT_PAGE_01  U1BDTP1
#define MCF_USB_OTG_BDT_PAGE_02  U1BDTP2
#define MCF_USB_OTG_BDT_PAGE_03  U1BDTP3

#define MCF_USB_OTG_TOKEN  U1TOK
#define MCF_USB_OTG_ENDPT0  U1EP0
#define MCF_USB_OTG_ENDPT_RETRY_DIS  _U1EP0_RETRYDIS_MASK
#define MCF_USB_OTG_CTL_HOST_MODE_EN  _U1CON_HOSTEN_MASK
#define MCF_USB_OTG_OTG_CTRL_DM_LOW  _U1OTGCON_DMPULDWN_MASK
#define MCF_USB_OTG_OTG_CTRL_DP_LOW  _U1OTGCON_DPPULDWN_MASK
#define MCF_USB_OTG_INT_ENB_ATTACH_EN  _U1IE_ATTACHIE_MASK
#define MCF_USB_OTG_INT_STAT_ATTACH  _U1IR_ATTACHIF_MASK
#define MCF_USB_OTG_CTL_JSTATE  _U1CON_JSTATE_MASK
#define MCF_USB_OTG_ADDR_LS_EN  _U1ADDR_LSPDEN_MASK
#define MCF_USB_OTG_CTL_RESET  _U1CON_USBRST_MASK
#define MCF_USB_OTG_INT_STAT_RESUME  _U1IR_RESUMEIF_MASK
#define MCF_USB_OTG_ENDPT_HOST_WO_HUB  _U1EP0_LSPD_MASK
#define MCF_USB_OTG_TOKEN_TOKEN_PID(x)  ((x)<<_U1TOK_PID0_POSITION)
#define MCF_USB_OTG_TOKEN_TOKEN_ENDPT(x)  ((x)<<_U1TOK_EP0_POSITION)
#define MCF_USB_OTG_INT_STAT_ERROR  _U1IE_UERRIE_MASK

/*
#define KVA_TO_PA(v)  ((v) & 0x1fffffff)
#define PA_TO_KVA0(pa)  ((pa) | 0x80000000)  // cachable
#define PA_TO_KVA1(pa)  ((pa) | 0xa0000000)
*/
#else
#define KVA_TO_PA(v)  (v)
#define PA_TO_KVA0(pa)  (pa)  // cachable
#define PA_TO_KVA1(pa)  (pa)
#endif

#define HWRETRIES  1
#define SWRETRIES  3

#define DEVICE_DESCRIPTOR_SIZE  18
#define CONFIGURATION_DESCRIPTOR_SIZE  128

#define BULK_ATTRIBUTES  2
#define INTERRUPT_ATTRIBUTES  3

#define FEATURE_ENDPOINT_HALT  0x00

#define TOKEN_OUT  0x01
#define TOKEN_ACK  0x02
#define TOKEN_DATA0  0x03
#define TOKEN_IN  0x09
#define TOKEN_NAK  0x0a
#define TOKEN_DATA1  0x0b
#define TOKEN_SETUP  0x0d

#define CLASS_CDC  0x02
#define CLASS_SCSI  0x08

#define BD_FLAGS_BC_ENC(x)  (((x) & 0x3ff) << 16)
#define BD_FLAGS_BC_DEC(y)  (((y) & 0x3ff0000) >> 16)
#define BD_FLAGS_OWN  0x80
#define BD_FLAGS_DATA  0x40
#define BD_FLAGS_DTS  0x08
#define BD_FLAGS_TOK_PID_DEC(y)  (((y) & 0x3c) >> 2)

#define MYBDT(endpoint, tx, odd)  (bdts+(endpoint)*4+(tx)*2+(odd))

#if PIC32
#define BDT_RAM_SIZE  256
#else
extern uint32 __BDT_RAM[], __BDT_RAM_END[];
#define BDT_RAM_SIZE  ((int)__BDT_RAM_END - (int)__BDT_RAM)
#endif

static struct bdt {
    int flags;
    byte *buffer;
} *bdts;  // 512 byte aligned in buffer

// N.B. only bdt endpoint 0 is used for host mode!

#if PICTOCRYPT || USB_HOST
#define ENDPOINTS  6
#else
#define ENDPOINTS  4
#endif

struct endpoint {
    byte toggle[2];  // rx [0] and tx [1] next packet data0 (0) or data1 (BD_FLAGS_DATA)
    byte bdtodd[2];  // rx [0] and tx [1] next bdt even (0) or odd (1)
    byte packetsize;
    bool inter;

    byte data_pid;  // TOKEN_IN -> data to host; TOKEN_OUT -> data from host
    int data_offset;  // current offset in data stream
    int data_length;  // max offset in data stream
    byte data_buffer[80];  // data to or from host
} endpoints[ENDPOINTS];

byte bulk_in_ep;
byte bulk_out_ep;
byte int_ep;

#if SODEBUG
volatile bool usb_in_isr;
volatile int32 usb_in_ticks;
volatile int32 usb_out_ticks;
volatile int32 usb_max_ticks;
#endif

bool cdc_attached;  // set when cdc acm device is attached
bool scsi_attached;  // set when usb mass storage device is attached
uint32 scsi_attached_count;
bool other_attached;  // set when other device is attached

bool cdcacm_attached;  // set when cdcacm host is attached
uint32 cdcacm_attached_count;

static
void
parse_configuration(const byte *configuration, int size)
{
    int i;

    // extract the bulk endpoint information
    for (i = 0; i < size; i += configuration[i]) {
        if (configuration[i+1] == ENDPOINT_DESCRIPTOR) {
            if (configuration[i+3] == BULK_ATTRIBUTES) {
                if (configuration[i+2] & 0x80) {
                    bulk_in_ep = (byte)(configuration[i+2] & 0xf);
                    assert(bulk_in_ep < LENGTHOF(endpoints));
                    assert(configuration[i+4]);
                    endpoints[bulk_in_ep].packetsize = configuration[i+4];
                } else {
                    bulk_out_ep = (byte)(configuration[i+2] & 0xf);
                    assert(bulk_out_ep < LENGTHOF(endpoints));
                    assert(configuration[i+4]);
                    endpoints[bulk_out_ep].packetsize = configuration[i+4];
                }
            } else if (configuration[i+3] == INTERRUPT_ATTRIBUTES) {
                int_ep = (byte)(configuration[i+2] & 0xf);
                assert(int_ep < LENGTHOF(endpoints));
                assert(configuration[i+4]);
                endpoints[int_ep].packetsize = configuration[i+4];
                endpoints[int_ep].inter = 1;
            }
        }
    }
    assert(i == size);
}

// *** host ***

#if ! STICKOS || USB_HOST
// initialize a setup data0 buffer
void
usb_setup(int in, int type, int recip, byte request, short value, short index, short length, struct setup *setup)
{
    assert(usb_host_mode);

    setup->requesttype = (byte)((in<<7)|(type << 5)|recip);
    setup->request = request;
    setup->value = TF_LITTLE(value);
    setup->index = TF_LITTLE(index);
    setup->length = TF_LITTLE(length);
}

// perform a usb host/device transaction
static
int
transaction(int endpoint, int token, void *buffer, int length)
{
    int bc;
    int tx;
    int pid;
    int odd;
    int stat;
    int toggle;
    int flags;
    int retry;
    int int_stat;
    volatile struct bdt *bdt;

    assert(usb_host_mode);

    if (token == TOKEN_SETUP) {
        tx = 1;

        // setup always uses data0; following transactions start with data1
        assert(! endpoint);
        endpoints[0].toggle[tx] = 0;
        endpoints[0].toggle[! tx] = BD_FLAGS_DATA;
    } else if (token == TOKEN_IN) {
        tx = 0;
    } else {
        assert(token == TOKEN_OUT);
        tx = 1;
    }

    toggle = endpoints[endpoint].toggle[tx];

    retry = 0;
    for (;;) {
        odd = endpoints[0].bdtodd[tx];
        endpoints[0].bdtodd[tx] = (byte)(! odd);

        // N.B. only bdt endpoint 0 is used for host mode!
        bdt = MYBDT(0, tx, odd);
        bdt->buffer = (byte *)TF_LITTLE(KVA_TO_PA((int)buffer));
        flags = TF_LITTLE(bdt->flags);
        assert(! (flags & BD_FLAGS_OWN));
        assert(length <= endpoints[endpoint].packetsize);
        bdt->flags = TF_LITTLE(BD_FLAGS_BC_ENC(length)|BD_FLAGS_OWN|toggle);

        assert(! (MCF_USB_OTG_CTL & MCF_USB_OTG_CTL_TXSUSPEND_TOKENBUSY));
        //MCF_USB_OTG_INT_STAT = MCF_USB_OTG_INT_STAT_STALL|MCF_USB_OTG_INT_STAT_TOK_DNE|MCF_USB_OTG_INT_STAT_USB_RST;

        MCF_USB_OTG_TOKEN = (uint8)(MCF_USB_OTG_TOKEN_TOKEN_PID(token)|MCF_USB_OTG_TOKEN_TOKEN_ENDPT(endpoint));

        // wait for stall or token done or reset
        for (;;) {
            int_stat = MCF_USB_OTG_INT_STAT;
            assert(! (int_stat & MCF_USB_OTG_INT_STAT_ERROR));
            if (int_stat & (/*MCF_USB_OTG_INT_STAT_STALL|*/MCF_USB_OTG_INT_STAT_TOK_DNE|MCF_USB_OTG_INT_STAT_USB_RST)) {
                break;
            }
        }

        stat = MCF_USB_OTG_STAT;
        flags = TF_LITTLE(bdt->flags);

        bc = BD_FLAGS_BC_DEC(flags);
        pid = BD_FLAGS_TOK_PID_DEC(flags);

        // if we got token done...
        if (int_stat & MCF_USB_OTG_INT_STAT_TOK_DNE) {
            MCF_USB_OTG_INT_STAT = MCF_USB_OTG_INT_STAT_TOK_DNE;
        }

        // if we got reset...
        if (int_stat & MCF_USB_OTG_INT_STAT_USB_RST) {
            MCF_USB_OTG_INT_STAT = MCF_USB_OTG_INT_STAT_USB_RST;
            // we must detach!!!
            return -1;
        }

        assert(! (flags & BD_FLAGS_OWN));

        if (pid) {
            assert(tx == !! (stat & MCF_USB_OTG_STAT_TX));
            assert(odd == !! (stat & MCF_USB_OTG_STAT_ODD));
            assert(0 == (stat >> 4));
        }

        switch (pid) {
            case TOKEN_DATA0:
            case TOKEN_DATA1:  // we received a data0/data1 packet
                assert(! tx);
                // if we expected it...
                if (pid == (toggle?TOKEN_DATA1:TOKEN_DATA0)) {
                    // flip the data toggle to acknowledge
                    endpoints[endpoint].toggle[tx] ^= BD_FLAGS_DATA;
                    return bc;
                } else {
                    goto XXX_RETRY_XXX;
                }
            case TOKEN_ACK:  // the device accepted the data packet we sent
                assert(tx);
                // flip the data toggle to acknowledge
                endpoints[endpoint].toggle[tx] ^= BD_FLAGS_DATA;
                return bc;
            case TOKEN_STALL:
                {
                    int rv;
                    struct setup setup;

                    if (endpoint) {
                        // clear the stall!
                        usb_setup(0, SETUP_TYPE_STANDARD, SETUP_RECIP_ENDPOINT, REQUEST_CLEAR_FEATURE, FEATURE_ENDPOINT_HALT, tx?endpoint:0x80|endpoint, 0, &setup);
                        rv = usb_control_transfer(&setup, NULL, 0);
                        assert(! rv);
                    }

                    endpoints[endpoint].toggle[0] = 0;
                    endpoints[endpoint].toggle[1] = 0;

                    return -TOKEN_STALL;
                }
            default:
                assert(0);
                // FALL THRU
            case 0:  // bus timeout
            case TOKEN_NAK:
            case 0xf:  // data error
XXX_RETRY_XXX:
                // ignore the packet and try again
                if (retry++ < SWRETRIES) {
                    delay(100);
                    continue;
                }
                //assert(0);
                return -15;
        }
    }
}

// perform a usb host/device control transfer
int
usb_control_transfer(struct setup *setup, byte *buffer, int length)
{
    int in;
    int rv;
    int total;
    int request;

    assert(usb_host_mode);

    rv = transaction(0, TOKEN_SETUP, setup, SETUP_SIZE);
    if (rv < 0) {
        return rv;
    }
    assert(rv == SETUP_SIZE);

    in = !! (setup->requesttype & 0x80);
    assert(in ? length : 1);  // if you don't have a length, use out!

    total = 0;
    while (total < length) {
        request = MIN(endpoints[0].packetsize, length-total);
        rv = transaction(0, in?TOKEN_IN:TOKEN_OUT, buffer+total, request);
        if (rv < 0) {
            return rv;
        }
        total += rv;
        if (rv < request) {
            break;
        }
    }

    // N.B. we always switch directions from the previous transaction
    // N.B. the new direction was initialized to data1 at setup time
    rv = transaction(0, in?TOKEN_OUT:TOKEN_IN, NULL, 0);
    if (rv < 0) {
        return rv;
    }
    assert(! rv);

    return total;
}

// perform a usb host/device bulk transfer
int
usb_bulk_transfer(int in, byte *buffer, int length, bool null_or_short)
{
    int rv;
    int total;
    int request;
    int endpoint;
    uint8 endpt0;

    assert(usb_host_mode);

    endpt0 = MCF_USB_OTG_ENDPT0;
    if (in == -1 || ! HWRETRIES) {
        MCF_USB_OTG_ENDPT0 |= MCF_USB_OTG_ENDPT_RETRY_DIS;
    }

    if (in == -1) {
        endpoint = int_ep;
    } else if (in) {
        endpoint = bulk_in_ep;
    } else {
        endpoint = bulk_out_ep;
    }
    assert(endpoint);
    assert(endpoints[endpoint].packetsize);

    total = 0;
    while (total < length) {
        request = MIN(endpoints[endpoint].packetsize, length-total);
        rv = transaction(endpoint, in?TOKEN_IN:TOKEN_OUT, buffer+total, request);
        if (rv < 0) {
            total = rv;
            break;
        }
        total += rv;
        if (rv < request) {
            break;
        }
    }

    // if the caller wants to end with a null or short packet and we ended
    // with a full packet...
    if (null_or_short && rv == endpoints[endpoint].packetsize) {
        rv = transaction(endpoint, in?TOKEN_IN:TOKEN_OUT, NULL, 0);
        if (rv < 0) {
            total = rv;
        }
    }

    MCF_USB_OTG_ENDPT0 = endpt0;

    return total;
}

// detach from the device and prepare to re-attach
void
usb_host_detach()
{
    assert(usb_host_mode);

    delay(100);  // debounce

    // enable usb pull downs
    MCF_USB_OTG_OTG_CTRL = MCF_USB_OTG_OTG_CTRL_DM_LOW|MCF_USB_OTG_OTG_CTRL_DP_LOW|MCF_USB_OTG_OTG_CTRL_OTG_EN;
#if STARTER
    // usb power on
    MCF_USB_OTG_OTG_CTRL |= _U1OTGCON_VBUSON_MASK;
#endif
#if DEMO && MCF52259
    MCF_GPIO_PQSPAR |= MCF_GPIO_PQSPAR_QSPI_CS2_USB_DM_PDOWN | MCF_GPIO_PQSPAR_QSPI_CS3_USB_DP_PDOWN;
#endif

    // enable host mode
    MCF_USB_OTG_CTL = MCF_USB_OTG_CTL_ODD_RST;
    MCF_USB_OTG_CTL = MCF_USB_OTG_CTL_HOST_MODE_EN;

    memset(bdts, 0, BDT_RAM_SIZE);
    memset(endpoints, 0, sizeof(endpoints));

    cdc_attached = 0;
    scsi_attached = 0;
    other_attached = 0;

    MCF_USB_OTG_INT_STAT = 0xff;
    MCF_USB_OTG_INT_STAT = 0xff;
    MCF_USB_OTG_INT_ENB |= MCF_USB_OTG_INT_ENB_ATTACH_EN;
}
#endif

// *** device ***

static const byte *device_descriptor;
static int device_descriptor_length;

static const byte *configuration_descriptor;
static int configuration_descriptor_length;

static const byte *string_descriptor;
static int string_descriptor_length;

static usb_reset_cbfn reset_cbfn;
static usb_control_cbfn control_transfer_cbfn;
static usb_bulk_cbfn bulk_transfer_cbfn;

// this function puts our state machine in a waiting state, waiting
// for a usb reset from the host.
static
void
usb_device_wait()
{
    assert(! usb_host_mode);

    // enable usb device mode
    MCF_USB_OTG_CTL = MCF_USB_OTG_CTL_USB_EN_SOF_EN;

    // enable usb pull ups
#if ! MCF51JM128
    MCF_USB_OTG_OTG_CTRL = MCF_USB_OTG_OTG_CTRL_DP_HIGH|MCF_USB_OTG_OTG_CTRL_OTG_EN;
#else
    USB_OTG_CONTROL |= USB_OTG_CONTROL_DPPULLUP_NONOTG_MASK;
    USBTRC0 |= USBTRC0_USBPU_MASK;
#endif

    // enable (only) usb reset interrupt
    MCF_USB_OTG_INT_STAT = 0xff;
    MCF_USB_OTG_INT_ENB = MCF_USB_OTG_INT_ENB_USB_RST_EN;
}

// this function puts our state machine into the default state,
// waiting for a "set configuration" command from the host.
static
void
usb_device_default()
{
    assert(! usb_host_mode);

    // default to address 0 on reset
    MCF_USB_OTG_ADDR = (uint8)0;

    // enable usb device mode
    MCF_USB_OTG_CTL |= MCF_USB_OTG_CTL_ODD_RST;
    MCF_USB_OTG_CTL &= ~MCF_USB_OTG_CTL_ODD_RST;

    memset(bdts, 0, BDT_RAM_SIZE);
    memset(endpoints, 0, sizeof(endpoints));

    assert(configuration_descriptor);

    // extract the maximum packet size from the device descriptor
    endpoints[0].packetsize = device_descriptor[7];

    // parse the configuration descriptor
    parse_configuration(configuration_descriptor, configuration_descriptor_length);

    // enable (also) usb sleep and token done interrupts
    MCF_USB_OTG_INT_STAT = 0xff;
    MCF_USB_OTG_INT_ENB |= MCF_USB_OTG_INT_ENB_SLEEP_EN|MCF_USB_OTG_INT_ENB_TOK_DNE_EN;
}

// enqueue a packet to the usb engine for transfer to/from the host
void
usb_device_enqueue(int endpoint, bool tx, byte *buffer, int length)
{
    int ep;
    bool odd;
    int flags;
    volatile struct bdt *bdt;

    assert(! usb_host_mode);
    assert(endpoint < LENGTHOF(endpoints));

    if (tx != (bool)-1) {
        // transfer up to one packet at a time
        assert(endpoints[endpoint].packetsize);
        length = MIN(length, endpoints[endpoint].packetsize);

        // find the next bdt entry to use
        odd = endpoints[endpoint].bdtodd[tx];

        // initialize the bdt entry
        bdt = MYBDT(endpoint, tx, odd);
        bdt->buffer = (byte *)TF_LITTLE(KVA_TO_PA((int)buffer));
        flags = TF_LITTLE(bdt->flags);
        assert(! (flags & BD_FLAGS_OWN));
        assert(length <= endpoints[endpoint].packetsize);
        bdt->flags = TF_LITTLE(BD_FLAGS_BC_ENC(length)|BD_FLAGS_OWN|endpoints[endpoint].toggle[tx]|BD_FLAGS_DTS);
    }

    ep = MCF_USB_OTG_ENDPT_EP_HSHK|MCF_USB_OTG_ENDPT_EP_TX_EN|MCF_USB_OTG_ENDPT_EP_RX_EN;
    ep |= endpoint?MCF_USB_OTG_ENDPT_EP_CTL_DIS:0;
    // enable the packet transfer
#if PIC32
    switch (endpoint) {
        case 0:
            U1EP0 = (uint8)(ep);
            break;
        case 1:
            U1EP1 = (uint8)(ep);
            break;
        case 2:
            U1EP2 = (uint8)(ep);
            break;
        case 3:
            U1EP3 = (uint8)(ep);
            break;
        default:
            ASSERT(0);
            break;
    }
#else
    MCF_USB_OTG_ENDPT(endpoint) = (uint8)(ep);
#endif
}

static byte setup_buffer[SETUP_SIZE];  // from host
static byte next_address;  // set after successful status

// *** isr ***

static byte descriptor[DEVICE_DESCRIPTOR_SIZE];
static byte configuration[CONFIGURATION_DESCRIPTOR_SIZE];

// called by usb on device attach
INTERRUPT
void
#if PIC32
#if defined(__32MX250F128B__)
__ISR(30, ipl6) // REVISIT -- ipl?
#else
__ISR(45, ipl6) // REVISIT -- ipl?
#endif
#endif
usb_isr(void)
{
#if ! STICKOS || USB_HOST
    int e;
#endif
    int rv;

    if (! bdts) {
        return;  // revisit
    }
    
    assert(! usb_in_isr);
    assert((usb_in_isr = true) ? true : true);
    assert((usb_in_ticks = ticks) ? true : true);
    
#if PIC32
#if PIC32PPS
    mUSBClearIntFlag();
#else
    IFS1CLR = 0x02000000; // USBIF
#endif
#else
     (void)splx(7);
#endif
    
    // *** host ***
    
#if ! STICKOS || USB_HOST
    if (MCF_USB_OTG_INT_STAT & MCF_USB_OTG_INT_STAT_ATTACH) {
        int size;
        struct setup setup;

        assert(usb_host_mode);
        
        delay(100);  // debounce
        MCF_USB_OTG_INT_STAT = MCF_USB_OTG_INT_STAT_ATTACH;
        delay(10);  // debounce
        
        // if this attach is not real...
        if (! (MCF_USB_OTG_INT_STAT & MCF_USB_OTG_INT_STAT_ATTACH)) {
            goto XXX_SKIP_XXX;
        }
        
        MCF_USB_OTG_INT_ENB &= ~MCF_USB_OTG_INT_ENB_ATTACH_EN;

#if PICTOCRYPT
        led_unknown();
#endif

        // default address 0 on attach
        MCF_USB_OTG_ADDR = (uint8)0;

        // if this is a low speed device...
        if (! (MCF_USB_OTG_CTL & MCF_USB_OTG_CTL_JSTATE)) {
            MCF_USB_OTG_ADDR |= MCF_USB_OTG_ADDR_LS_EN;
        }

        // reset the device
        MCF_USB_OTG_CTL |= MCF_USB_OTG_CTL_RESET;
        delay(10);
        MCF_USB_OTG_CTL &= ~MCF_USB_OTG_CTL_RESET;

        // enable sof
        MCF_USB_OTG_CTL |= MCF_USB_OTG_CTL_USB_EN_SOF_EN;
        MCF_USB_OTG_INT_STAT = MCF_USB_OTG_INT_STAT_SLEEP|MCF_USB_OTG_INT_STAT_RESUME;

        delay(100);  // post reset
        MCF_USB_OTG_INT_STAT = MCF_USB_OTG_INT_STAT_USB_RST;

        // enable transfers
        MCF_USB_OTG_ENDPT0 = MCF_USB_OTG_ENDPT_HOST_WO_HUB/*|MCF_USB_OTG_ENDPT_RETRY_DIS*/;
        MCF_USB_OTG_ENDPT0 |= (uint8)(MCF_USB_OTG_ENDPT_EP_HSHK|MCF_USB_OTG_ENDPT_EP_TX_EN|MCF_USB_OTG_ENDPT_EP_RX_EN);

        // data0 follows configuration event
        memset(endpoints, 0, sizeof(endpoints));

        // default packetsize
        endpoints[0].packetsize = 8;

        // get the first 8 bytes of the device descriptor
        usb_setup(1, SETUP_TYPE_STANDARD, SETUP_RECIP_DEVICE, REQUEST_GET_DESCRIPTOR, (DEVICE_DESCRIPTOR<<8)|0, 0, 8, &setup);
        rv = usb_control_transfer(&setup, descriptor, 8);
        if (rv < 0) {
            usb_host_detach();
            goto XXX_SKIP_XXX;
        }
        assert(rv == 8);

#if PICTOCRYPT
        led_unknown_progress();
#endif

        // extract the maximum packet size
        endpoints[0].packetsize = descriptor[7];

        // then get the whole device descriptor
        usb_setup(1, SETUP_TYPE_STANDARD, SETUP_RECIP_DEVICE, REQUEST_GET_DESCRIPTOR, (DEVICE_DESCRIPTOR<<8)|0, 0, sizeof(descriptor), &setup);
        rv = usb_control_transfer(&setup, descriptor, sizeof(descriptor));
        assert(rv >= 8);

        // set address to 1
        usb_setup(0, SETUP_TYPE_STANDARD, SETUP_RECIP_DEVICE, REQUEST_SET_ADDRESS, 1, 0, 0, &setup);
        rv = usb_control_transfer(&setup, NULL, 0);
        assert(rv == 0);
        MCF_USB_OTG_ADDR |= 1;

        delay(100);  // post set address recovery

        // get the first 9 bytes of the configuration descriptor
        usb_setup(1, SETUP_TYPE_STANDARD, SETUP_RECIP_DEVICE, REQUEST_GET_DESCRIPTOR, (CONFIGURATION_DESCRIPTOR<<8)|0, 0, 9, &setup);
        rv = usb_control_transfer(&setup, configuration, 9);
        assert(rv > 0);

        size = configuration[2];
        assert(size >= 9 && size < sizeof(configuration));

        // then get the whole configuration descriptor
        usb_setup(1, SETUP_TYPE_STANDARD, SETUP_RECIP_DEVICE, REQUEST_GET_DESCRIPTOR, (CONFIGURATION_DESCRIPTOR<<8)|0, 0, size, &setup);
        rv = usb_control_transfer(&setup, configuration, size);
        assert(rv >= 17);

        // parse it
        parse_configuration(configuration, size);

        // set configuration
        usb_setup(0, SETUP_TYPE_STANDARD, SETUP_RECIP_DEVICE, REQUEST_SET_CONFIGURATION, configuration[5], 0, 0, &setup);
        rv = usb_control_transfer(&setup, NULL, 0);
        assert(rv == 0);
        
        delay(200);  // post set configuration recovery
        
        for (e = 1; e < LENGTHOF(endpoints); e++) {
            assert(endpoints[e].toggle[0] == 0);
            assert(endpoints[e].toggle[1] == 0);
        }

        if (bulk_in_ep && bulk_out_ep && (descriptor[4] == CLASS_SCSI || (descriptor[4] == 0x00 && configuration[9+5] == CLASS_SCSI))) {
            assert(configuration[9+6] == 0x01 || configuration[9+6] == 0x05 || configuration[9+6] == 0x06);  // RBC or SFF or transparent
            assert(configuration[9+7] == 0x50);  // bulk-only
            scsi_attached_count++;
            scsi_attached = 1+(configuration[9+6] != 0x06);
        } else if (bulk_in_ep && bulk_out_ep && (descriptor[4] == CLASS_CDC || (descriptor[4] == 0x00 && configuration[9+5] == CLASS_CDC))) {
            assert(configuration[9+6] == 0x02);  // ACM
            // assert(configuration[9+7] == 0x01);  // AT
            cdc_attached = 1;
        } else {
            other_attached = 1;
        }
        
        // enable the detach interrupt
        MCF_USB_OTG_INT_ENB |= MCF_USB_OTG_INT_ENB_USB_RST_EN;

        led_unknown_progress();
    }
#endif
    
    // *** device ***
    
    // if we just transferred a token...
    if (MCF_USB_OTG_INT_STAT & MCF_USB_OTG_INT_STAT_TOK_DNE) {
        int bc;
        int tx;
        int odd;
        int pid;
        int stat;
        int flags;
        byte *data;
        int endpoint;
        int endpoint2;
        short length;
        short value;
        volatile struct bdt *bdt;
        struct setup *setup;
        
        assert(! usb_host_mode);

        // we just completed a packet transfer
        stat = MCF_USB_OTG_STAT;
        tx = !! (stat & MCF_USB_OTG_STAT_TX);
        odd = !! (stat & MCF_USB_OTG_STAT_ODD);
        endpoint = (stat & 0xf0) >> 4;

        // toggle the data toggle flag
        endpoints[endpoint].toggle[tx] = endpoints[endpoint].toggle[tx] ? 0 : BD_FLAGS_DATA;
        
        // toggle the next bdt entry to use
        ASSERT(odd == endpoints[endpoint].bdtodd[tx]);
        endpoints[endpoint].bdtodd[tx] = ! endpoints[endpoint].bdtodd[tx];

        bdt = MYBDT(endpoint, tx, odd);

        flags = TF_LITTLE(bdt->flags);
        assert(! (flags & BD_FLAGS_OWN));

        bc = BD_FLAGS_BC_DEC(flags);
        assert(bc >= 0);

        pid = BD_FLAGS_TOK_PID_DEC(flags);

        // if we're starting a new control transfer...
        if (pid == TOKEN_SETUP) {
            assert(! endpoint);
            assert(bc == 8);
            assert(! tx);

            setup = (struct setup *)TF_LITTLE((int)PA_TO_KVA1((int)bdt->buffer));
            assert((void *)setup == (void *)setup_buffer);

            // unsuspend the usb packet engine
            MCF_USB_OTG_CTL &= ~MCF_USB_OTG_CTL_TXSUSPEND_TOKENBUSY;

            length = TF_LITTLE(setup->length);

            endpoints[endpoint].data_pid = TOKEN_OUT;
            endpoints[endpoint].data_length = 0;
            endpoints[endpoint].data_offset = 0;

            // is this a standard command...
            if (! (setup->requesttype & 0x60)) {
                value = TF_LITTLE(setup->value);
                if (setup->request == REQUEST_GET_DESCRIPTOR) {
                    endpoints[endpoint].data_pid = TOKEN_IN;

                    if ((value >> 8) == DEVICE_DESCRIPTOR) {
                        assert(device_descriptor_length);
                        endpoints[endpoint].data_length = MIN(device_descriptor_length, length);
                        memcpy(endpoints[endpoint].data_buffer, device_descriptor, endpoints[endpoint].data_length);
                    } else if ((value >> 8) == CONFIGURATION_DESCRIPTOR) {
                        assert(configuration_descriptor_length);
                        endpoints[endpoint].data_length = MIN(configuration_descriptor_length, length);
                        memcpy(endpoints[endpoint].data_buffer, configuration_descriptor, endpoints[endpoint].data_length);
                    } else if ((value >> 8) == STRING_DESCRIPTOR) {
                        int i;
                        int j;

                        // find the string descriptor
                        i = value & 0xff;
                        j = 0;
                        while (i-- && j < string_descriptor_length) {
                            j += string_descriptor[j];
                        }
                        if (i != -1) {
                            assert(j == string_descriptor_length);
                            endpoints[endpoint].data_length = 0;  // what to return here?
                        } else {
                            assert(string_descriptor[j]);
                            endpoints[endpoint].data_length = MIN(string_descriptor[j], length);
                            memcpy(endpoints[endpoint].data_buffer, string_descriptor+j, endpoints[endpoint].data_length);
                        }
                    } else if ((value >> 8) == DEVICE_QUALIFIER_DESCRIPTOR) {
                        endpoints[endpoint].data_length = 0;
                    } else {
                        assert(0);
                    }

                    // data phase starts with data1
                    assert(endpoints[endpoint].toggle[1] == BD_FLAGS_DATA);
                    usb_device_enqueue(0, 1, endpoints[endpoint].data_buffer, MIN(endpoints[endpoint].data_length, endpoints[endpoint].packetsize));
                } else {
                    if (setup->request == REQUEST_CLEAR_FEATURE) {
                        assert(! length);
                        // if we're recovering from an error...
                        if (setup->requesttype == 0x02 && ! value) {
                            endpoint2 = TF_LITTLE(setup->index) & 0x0f;
                            assert(endpoint2);
                            // clear the data toggle
                            endpoints[endpoint2].toggle[0] = 0;
                            endpoints[endpoint2].toggle[1] = 0;
                        } else {
                            assert(0);
                        }
                    } else if (setup->request == REQUEST_SET_ADDRESS) {
                        next_address = value;
                    } else if (setup->request == REQUEST_SET_CONFIGURATION) {
                        assert(value == 1);
                        cdcacm_attached_count++;
                        cdcacm_attached = 1;
                    } else if (setup->request == REQUEST_GET_CONFIGURATION) {
                        endpoints[endpoint].data_pid = TOKEN_IN;

                        endpoints[endpoint].data_length = 1;
                        endpoints[endpoint].data_buffer[0] = 1;

                        // data phase starts with data1
                        assert(endpoints[endpoint].toggle[1] == BD_FLAGS_DATA);
                        usb_device_enqueue(0, 1, endpoints[endpoint].data_buffer, MIN(endpoints[endpoint].data_length, endpoints[endpoint].packetsize));
                        goto XXX_SKIP2_XXX;
                    } else {
                        assert(0);
                    }

                    // prepare to transfer status (in the other direction)
                    usb_device_enqueue(0, 1, NULL, 0);
XXX_SKIP2_XXX:;
                }
            // otherwise, this is a class or vendor command
            } else {
                if (setup->requesttype & 0x80/*in*/) {
                    // host wants to receive data, get it from our caller!
                    assert(control_transfer_cbfn);
                    rv = control_transfer_cbfn(setup, endpoints[endpoint].data_buffer, length);
                    assert(rv >= 0);
                    assert(rv <= length);

                    // prepare to send data, TOKEN_IN(s) will follow
                    endpoints[endpoint].data_pid = TOKEN_IN;
                    assert(rv > 0);  // if you don't have a length, use out!
                    endpoints[endpoint].data_length = rv;
                    usb_device_enqueue(0, 1, endpoints[endpoint].data_buffer, endpoints[endpoint].data_length);
                } else {
                    // host is sending data
                    if (length) {
                        // we will receive data, TOKEN_OUT(s) will follow
                        endpoints[endpoint].data_length = length;
                        usb_device_enqueue(0, 0, endpoints[endpoint].data_buffer, endpoints[endpoint].packetsize);
                    } else {
                        // data transfer is done; put it to our caller!
                        assert(control_transfer_cbfn);
                        rv = control_transfer_cbfn((struct setup *)setup_buffer, NULL, 0);
                        assert(rv != -1);

                        // status uses data1
                        assert(endpoints[endpoint].toggle[1] == BD_FLAGS_DATA);

                        // prepare to transfer status (in the other direction)
                        usb_device_enqueue(0, 1, NULL, 0);
                    }
                }
            }
            assert((unsigned)endpoint < LENGTHOF(endpoints));
            assert(endpoints[endpoint].data_length <= sizeof(endpoints[endpoint].data_buffer));
        } else if (! endpoint) {
            assert(pid == TOKEN_IN || pid == TOKEN_OUT);
            data = (byte *)TF_LITTLE((int)PA_TO_KVA1((int)bdt->buffer));

            // if this is part of the data transfer...
            if (pid == endpoints[endpoint].data_pid) {
                assert((char *)data >= (char *)endpoints[endpoint].data_buffer && (char *)data < (char *)endpoints[endpoint].data_buffer+sizeof(endpoints[endpoint].data_buffer));
                if (pid == TOKEN_IN) {
                    assert(tx);
                    // we just sent data to the host
                    endpoints[endpoint].data_offset += bc;
                    assert(endpoints[endpoint].data_offset <= endpoints[endpoint].data_length);

                    // if there's more data to send...
                    if (endpoints[endpoint].data_offset != endpoints[endpoint].data_length) {
                        // send it
                        usb_device_enqueue(0, 1, endpoints[endpoint].data_buffer+endpoints[endpoint].data_offset, endpoints[endpoint].data_length-endpoints[endpoint].data_offset);
                    } else {
                        // status uses data1
                        assert(endpoints[endpoint].toggle[0] == BD_FLAGS_DATA);

                        // prepare to transfer status (in the other direction)
                        usb_device_enqueue(0, 0, NULL, 0);
                    }
                } else {
                    assert(! tx);
                    // we just received data from the host
                    endpoints[endpoint].data_offset += bc;
                    assert(endpoints[endpoint].data_offset <= endpoints[endpoint].data_length);

                    // if there's more data to receive...
                    if (endpoints[endpoint].data_offset != endpoints[endpoint].data_length) {
                        // receive it
                        usb_device_enqueue(0, 0, endpoints[endpoint].data_buffer+endpoints[endpoint].data_offset, endpoints[endpoint].data_length-endpoints[endpoint].data_offset);
                    } else {
                        // put it to our caller!
                        assert(control_transfer_cbfn);
                        rv = control_transfer_cbfn((struct setup *)setup_buffer, endpoints[endpoint].data_buffer, endpoints[endpoint].data_length);
                        assert(rv != -1);

                        // status uses data1
                        assert(endpoints[endpoint].toggle[1] == BD_FLAGS_DATA);

                        // prepare to transfer status (in the other direction)
                        usb_device_enqueue(0, 1, NULL, 0);
                    }
                }
            // otherwise; we just transferred status
            } else {
                assert(data == PA_TO_KVA1(0));

                // update our address after status
                if (next_address) {
                    MCF_USB_OTG_ADDR |= next_address;
                    next_address = 0;
                }

                // setup always uses data0; following transactions start with data1
                endpoints[endpoint].toggle[0] = 0;
                endpoints[endpoint].toggle[1] = BD_FLAGS_DATA;

                // prepare to receive setup token
                usb_device_enqueue(0, 0, setup_buffer, sizeof(setup_buffer));
            }
        } else if (endpoint != int_ep) {
            assert(pid == TOKEN_IN || pid == TOKEN_OUT);
            data = (byte *)TF_LITTLE((int)PA_TO_KVA1((int)bdt->buffer));

            // we just received or sent data from or to the host
            assert(bulk_transfer_cbfn);
            bulk_transfer_cbfn(pid == TOKEN_IN, data, bc);
        }

        MCF_USB_OTG_INT_STAT = MCF_USB_OTG_INT_STAT_TOK_DNE;
    }

    // if we just got reset by the host...
    if (MCF_USB_OTG_INT_STAT & MCF_USB_OTG_INT_STAT_USB_RST) {
        if (usb_host_mode) {
#if USB_HOST
            usb_host_detach();
#endif
        } else {
            cdcacm_active = 0;
            cdcacm_attached = 0;
    
            usb_device_default();
    
            assert(reset_cbfn);
            reset_cbfn();
    
            // setup always uses data0; following transactions start with data1
            endpoints[0].toggle[0] = 0;
            endpoints[0].toggle[1] = BD_FLAGS_DATA;
    
            // prepare to receive setup token
            usb_device_enqueue(0, 0, setup_buffer, sizeof(setup_buffer));
        }

        led_unknown_progress();

        MCF_USB_OTG_INT_STAT = MCF_USB_OTG_INT_STAT_USB_RST;
    }

    // if we just went idle...
    if (MCF_USB_OTG_INT_STAT & MCF_USB_OTG_INT_STAT_SLEEP) {
        assert(! usb_host_mode);
    
        cdcacm_active = 0;
        cdcacm_attached = 0;

        // disable usb sleep interrupts
        MCF_USB_OTG_INT_ENB &= ~MCF_USB_OTG_INT_ENB_SLEEP_EN;
        MCF_USB_OTG_INT_STAT = MCF_USB_OTG_INT_STAT_SLEEP;
    }
    
XXX_SKIP_XXX:
    assert(usb_in_isr);
    assert((usb_in_isr = false) ? true : true);
    assert((usb_out_ticks = ticks) ? true : true);
    assert((usb_max_ticks = MAX(usb_max_ticks, usb_out_ticks-usb_in_ticks)) ? true : true);
}

// this function is called by upper level code to register callback
// functions.
void
usb_register(usb_reset_cbfn reset, usb_control_cbfn control_transfer, usb_bulk_cbfn bulk_transfer)
{
    assert(! usb_host_mode);
    reset_cbfn = reset;
    control_transfer_cbfn = control_transfer;
    bulk_transfer_cbfn = bulk_transfer;
}

// called by upper level code to specify the device descriptor to
// return to the host.
void
usb_device_descriptor(const byte *descriptor, int length)
{
    assert(! usb_host_mode);
    device_descriptor = descriptor;
    device_descriptor_length = length;
}

// called by upper level code to specify the configuration descriptor
// to return to the host.
void
usb_configuration_descriptor(const byte *descriptor, int length)
{
    assert(! usb_host_mode);
    configuration_descriptor = descriptor;
    configuration_descriptor_length = length;
}

// called by upper level code to specify the string descriptors to
// return to the host.
void
usb_string_descriptor(const byte *descriptor, int length)
{
    assert(! usb_host_mode);
    string_descriptor = descriptor;
    string_descriptor_length = length;
}

void
usb_initialize(void)
{
#if PIC32
    static __attribute__ ((aligned(512))) byte bdt_ram[BDT_RAM_SIZE];

    bdts = (struct bdt *)bdt_ram;
#else
    bdts = (struct bdt *)__BDT_RAM;
#endif

    assert(BDT_RAM_SIZE >= LENGTHOF(endpoints)*4*sizeof(struct bdt));

#if MCF52221 || MCF52259
    // enable usb interrupt
    MCF_INTC0_ICR53 = MCF_INTC_ICR_IL(SPL_USB)|MCF_INTC_ICR_IP(SPL_USB);
    MCF_INTC0_IMRL &= ~MCF_INTC_IMRL_MASKALL;
    MCF_INTC0_IMRH &= ~MCF_INTC_IMRH_INT_MASK53;  // usb
#elif MCF51JM128
    /* Reset USB module first. */
    USBTRC0_USBRESET = 1;
    while (USBTRC0_USBRESET ) {
        // NULL
    }
    USBTRC0 |= USBTRC0_USBVREN_MASK;
#elif PIC32
    // power on
    U1PWRCbits.USBPWR = 1;

    // enable int
#if PIC32PPS
    mUSBIntEnable(true);
    mUSBSetIntPriority(6);
    mUSBSetIntSubPriority(0);
    //IEC1bits.USBIE = 1;
    //IPC7bits.USBIP = 6;
    //IPC7bits.USBIS = 0;
#else
    IEC1bits.USBIE = 1;
    IPC11bits.USBIP = 6;
    IPC11bits.USBIS = 0;
#endif
    //INTEnable(INT_USB, 1);
    //INTSetPriority(INT_USB, 6);
#endif

#if MCF52221 || MCF52259 || MCF51JM128
    // initialize usb timing
    if (oscillator_frequency == 48000000) {
        MCF_USB_OTG_USB_CTRL = MCF_USB_OTG_USB_CTRL_CLK_SRC(1);
    } else {
        assert(cpu_frequency == 48000000);
        MCF_USB_OTG_USB_CTRL = MCF_USB_OTG_USB_CTRL_CLK_SRC(3);
    }
#endif
    MCF_USB_OTG_SOF_THLD = 74;

    // initialize usb bdt
    assert(! ((unsigned int)bdts & 0x1ff));
    MCF_USB_OTG_BDT_PAGE_01 = (uint8)(KVA_TO_PA((unsigned int)bdts) >> 8);
    MCF_USB_OTG_BDT_PAGE_02 = (uint8)(KVA_TO_PA((unsigned int)bdts) >> 16);
    MCF_USB_OTG_BDT_PAGE_03 = (uint8)(KVA_TO_PA((unsigned int)bdts) >> 24);

    // if we are in host mode...
    if (usb_host_mode) {
#if DEMO
        // usb power on
        MCF_GPIO_PUAPAR &= ~0xc0;
        MCF_GPIO_DDRUA = 0x08;
        MCF_GPIO_CLRUA = (uint8)~0x08;
#endif

#if ! STICKOS || USB_HOST
        // enable usb to interrupt on attach
        usb_host_detach();
#else
        ASSERT(0);
#endif
    } else {
        // enable usb to interrupt on reset
        usb_device_wait();
    }
}
#endif

