// *** qspi.c *********************************************************
// this file performs qspi master i/o transfers.

#include "main.h"

#define QSPI_BAUD_FAST  800000  // zigflea
#define QSPI_BAUD_SLOW  200000  // default

// perform both output and input qspi i/o
void
qspi_transfer(bool cs, byte *buffer, int length)
{
    int x;
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    int i;
    int request;
#endif

    qspi_initialize();

    x = splx(7);
    if (cs) {
        // cs active
        pin_set(pin_assignments[pin_assignment_qspi_cs], pin_type_digital_output, 0, 0);
    }
    
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    // while there is data remaining...
    while (length) {
        // process up to 16 bytes at a time
        request = MIN(length, 16);

        // for all bytes...
        for (i = 0; i < request; i++) {
            // set up the command
            MCF_QSPI_QAR = MCF_QSPI_QAR_CMD+i;
            MCF_QSPI_QDR = MCF_QSPI_QDR_CONT;

            // copy tx data to qspi ram
            MCF_QSPI_QAR = MCF_QSPI_QAR_TRANS+i;
            MCF_QSPI_QDR = buffer[i];
        }

        // set the queue pointers
        assert(request);
        MCF_QSPI_QWR = MCF_QSPI_QWR_ENDQP(request-1)|MCF_QSPI_QWR_NEWQP(0);

        // start the transfer
        MCF_QSPI_QDLYR = MCF_QSPI_QDLYR_SPE;

        // wait for transfer complete
        assert(! (MCF_QSPI_QIR & MCF_QSPI_QIR_SPIF));
        while (! (MCF_QSPI_QIR & MCF_QSPI_QIR_SPIF)) {
        }
        MCF_QSPI_QIR = MCF_QSPI_QIR_SPIF;

        assert((MCF_QSPI_QWR & 0xf0) >> 4 == request-1);
        assert(! (MCF_QSPI_QDLYR & MCF_QSPI_QDLYR_SPE));

        // for all bytes...
        for (i = 0; i < request; i++) {
            // copy rx data from qspi ram
            MCF_QSPI_QAR = MCF_QSPI_QAR_RECV+i;
            buffer[i] = MCF_QSPI_QDR;
        }

        buffer += request;
        length -= request;
    }

    // transfer complete
    MCF_QSPI_QWR = 0;
#elif MCF51JM128 || MCF51CN128 || MCF51QE128 || MC9S08QE128 || MC9S12DT256 || MC9S12DP512
    while (length) {
        // N.B. spi needs us to read the status register even for release code!
        ASSERT(SPI1SX & SPI1S_SPTEF_MASK);
        ASSERT(! (SPI1SX & SPI1S_SPRF_MASK));
        
        SPI1DL = *buffer;
        
        while (! (SPI1SX & SPI1S_SPTEF_MASK)) {
            // NULL
        }
        
        while (! (SPI1SX & SPI1S_SPRF_MASK)) {
            // NULL
        }
        
        *buffer = SPI1DL;
        
        buffer++;
        length--;
    }
#elif PIC32
    while (length) {
        assert(! (SPI2STATbits.SPIBUSY));
        assert(! (SPI2STATbits.SPIRBF));
        assert(SPI2STATbits.SPITBE);
        SPI2BUF = *buffer;
        
        while (! SPI2STATbits.SPIRBF) {
            // NULL
        }
        
        assert(! (SPI2STATbits.SPIBUSY));
        assert(SPI2STATbits.SPIRBF);
        assert(SPI2STATbits.SPITBE);
        *buffer = SPI2BUF;
        
        buffer++;
        length--;
    }
#endif

    if (cs) {
        // cs inactive
        pin_set(pin_assignments[pin_assignment_qspi_cs], pin_type_digital_output, 0, 1);
    }
    splx(x);
}

extern void
qspi_baud_fast(void)
{
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    // initialize qspi master at 800k baud
    assert(bus_frequency/QSPI_BAUD_FAST < 256);
    MCF_QSPI_QMR = MCF_QSPI_QMR_MSTR|/*MCF_QSPI_QMR_CPOL|MCF_QSPI_QMR_CPHA|*/bus_frequency/QSPI_BAUD_FAST;
#elif MCF51JM128 || MCF51CN128 || MCF51QE128 || MC9S08QE128 || MC9S12DT256 || MC9S12DP512
    int log2;
    int divisor;
    
    // initialize qspi master at 800k baud
    log2 = 0;
    divisor = bus_frequency/QSPI_BAUD_FAST/2;
    while (divisor > 8) {
        divisor /= 2;
        log2++;
    }
    assert(log2 < 8 && (divisor-1) < 8);
    SPI1BRX_SPR = log2;
    SPI1BRX_SPPR = divisor-1;
#elif PIC32
    // initialize qspi master at 800k baud
    SPI2CON = 0;

    assert(bus_frequency/QSPI_BAUD_FAST/2 - 1 < 512);
    SPI2BRG = bus_frequency/QSPI_BAUD_FAST/2 - 1;
    
    SPI2CON = _SPI2CON_ON_MASK|_SPI2CON_CKE_MASK|_SPI2CON_MSTEN_MASK;
#endif
}

static bool initialized;

extern void
qspi_uninitialize(void)
{
    initialized = false;

#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    // QS is gpio
    MCF_GPIO_PQSPAR = (MCF_GPIO_PQSPAR&0xffc0)|0x0000;
#elif MCF51JM128 || MCF51CN128 || MCF51QE128 || MC9S08QE128 || MC9S12DT256 || MC9S12DP512
    SPI1C1X = 0;
    SPI1C2X = 0;
#elif PIC32
    SPI2CON = 0;
#else
#error
#endif
}

extern void
qspi_initialize(void)
{
    if (initialized) {
        return;
    }
    initialized = true;

    {
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    // QS is primary
    MCF_GPIO_PQSPAR = (MCF_GPIO_PQSPAR&0xffc0)|0x0015;

    // initialize qspi master at 200k baud
    assert(bus_frequency/QSPI_BAUD_SLOW < 256);
    MCF_QSPI_QMR = MCF_QSPI_QMR_MSTR|/*MCF_QSPI_QMR_CPOL|MCF_QSPI_QMR_CPHA|*/bus_frequency/QSPI_BAUD_SLOW;
#elif MCF51JM128 || MCF51CN128 || MCF51QE128 || MC9S08QE128 || MC9S12DT256 || MC9S12DP512
    int log2;
    int divisor;
    
    SPI1C1X = SPI1C1_SPE_MASK|SPI1C1_MSTR_MASK;
    SPI1C2X = 0;
    
    // initialize qspi master at 200k baud
    log2 = 0;
    divisor = bus_frequency/QSPI_BAUD_SLOW/2;
    while (divisor > 8) {
        divisor /= 2;
        log2++;
    }
    assert(log2 < 8 && (divisor-1) < 8);
    SPI1BRX_SPR = log2;
    SPI1BRX_SPPR = divisor-1;
    
#if MCF51CN128
    PTCPF1 |= 0xa8;  // ptc5,6,7
#endif

#if MC9S12DT256 || MC9S12DP512
    // route spi0 to m2,3,4,5
    MODRR |= 0x10;
#endif
#elif PIC32
    SPI2CON = 0;

    assert(bus_frequency/QSPI_BAUD_SLOW/2 - 1 < 512);
    SPI2BRG = bus_frequency/QSPI_BAUD_SLOW/2 - 1;
    
    SPI2CON = _SPI2CON_ON_MASK|_SPI2CON_CKE_MASK|_SPI2CON_MSTEN_MASK;
#else
#error
#endif
    }
}

