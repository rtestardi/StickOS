// *** adc.c **********************************************************
// this file polls the analog-to-digital converters coupled with the
// an0-an7 pins when used in analog input mode.

#include "main.h"

static bool adc_initialized;

#if MCF51JM128 || MCF51CN128 || MCF51QE128 || MC9S08QE128
static byte adcn = -1;
#endif


enum {
    adc_num_debounce_history = 3
};


// updated every tick
#if ! PICTOCRYPT
static
#endif
volatile uint16 adc_result[adc_num_channel];  // 0..4095

// updated every "debouncing" tick, the rate is determined by timer_isr()
static
volatile uint16 adc_debounce[adc_num_channel][adc_num_debounce_history];

// indexes into adc_debounce: adc_debounce[ch][adc_debounce_set].
static
int adc_debounce_set;


// poll the analog-to-digital converters
void
adc_timer_poll(bool debouncing)
{
#if ! STICK_GUEST
#if MCF52221 || MCF52233 || MCF52259 || MCF5211 || MC9S12DT256 || MC9S12DP512 || PIC32
    int i;
#endif
#endif

    if (! adc_initialized) {
        return;
    }

    assert(adc_debounce_set < adc_num_debounce_history);

#if ! STICK_GUEST
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    // if all channels are ready...
    if ((MCF_ADC_ADSTAT & 0xff) == 0xff) {
        // read all channel results into adc_result[]
        for (i = 0; i < adc_num_channel; i++) {
            adc_result[i] = MCF_ADC_ADRSLT(i)>>3;  // 0..4095
            if (debouncing) {
                adc_debounce[i][adc_debounce_set] = adc_result[i];
            }
        }
    } else {
        // prevent debounce increment below because we did not read adc data and to populate the current debounce set.
        debouncing = false;
    }

    // re-start the adc
    MCF_ADC_CTRL1 = MCF_ADC_CTRL1_START0;
#elif MCF51JM128 || MCF51CN128 || MCF51QE128 || MC9S08QE128
    // we can only poll one enabled channel every tick
    
    // if a previous result just completed...
    if (adcn != (byte)-1 && (ADCSC1 & ADCSC1_COCO_MASK)) {
        assert(adcn < adc_num_channel);
        adc_result[adcn] = (ADCRH<<8)|ADCRL;  // 0..4095
        // REVISIT -- this assumes we can't keep up with the debounce cycle!
        adc_debounce[adcn][adc_debounce_set] = adc_result[adcn];
    }
    
    // initiate the next result
    // advance the debounce set if wrapping adcn
    if (++adcn >= adc_num_channel) {
        adcn = 0;
        debouncing = true;
    } else {
        debouncing = false;
    }
    ADCSC1 = adcn;
#elif MC9S12DT256 || MC9S12DP512
    // if all channels are ready...
    if (ATD0STAT0_SCF && ATD1STAT0_SCF) {
        // read all channel results into adc_result[]
        adc_result[0] = ATD0DR0<<2;
        adc_result[1] = ATD0DR1<<2;
        adc_result[2] = ATD0DR2<<2;
        adc_result[3] = ATD0DR3<<2;
        adc_result[4] = ATD0DR4<<2;
        adc_result[5] = ATD0DR5<<2;
        adc_result[6] = ATD0DR6<<2;
        adc_result[7] = ATD0DR7<<2;
        adc_result[8] = ATD1DR0<<2;
        adc_result[9] = ATD1DR1<<2;
        adc_result[10] = ATD1DR2<<2;
        adc_result[11] = ATD1DR3<<2;
        adc_result[12] = ATD1DR4<<2;
        adc_result[13] = ATD1DR5<<2;
        adc_result[14] = ATD1DR6<<2;
        adc_result[15] = ATD1DR7<<2;
        
        if (debouncing) {
            for (i = 0; i < adc_num_channel; i++) {
                adc_debounce[i][adc_debounce_set] = adc_result[i];
            }
        }
    } else {
        // prevent debounce increment below because we did not read adc data and to populate the current debounce set.
        debouncing = false;
    }

    // re-start the adc
    /* ATD0CTL5: DJM=1,DSGN=0,SCAN=0,MULT=1,??=0,CC=0,CB=0,CA=0 */
    ATD0CTL5 = 144;                      /* Start conversions */
    ATD1CTL5 = 144;                      /* Start conversions */
#elif PIC32
    for (i = 0; i < adc_num_channel; i++) {
        adc_result[i] = ReadADC10(i)<<2;  // 0..4095
        if (debouncing) {
            adc_debounce[i][adc_debounce_set] = adc_result[i];
        }
    }
#else
#error
#endif
#endif // ! STICK_GUEST

    // if a debounced set if full, then advance the debouncing index to the next set.
    if (debouncing) {
        if (++adc_debounce_set >= adc_num_debounce_history) {
            adc_debounce_set = 0;
        }
    }
}

void
adc_sleep()
{
#if ! STICK_GUEST
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    // power off the adc
    MCF_ADC_POWER = MCF_ADC_POWER_PUDELAY(13)|MCF_ADC_POWER_PD2|MCF_ADC_POWER_PD1|MCF_ADC_POWER_PD0;  // disable adc
#endif
#endif // ! STICK_GUEST
}

int
adc_get_value(int offset, int pin_qual)
{
    int value;

    assert(offset < adc_num_channel);

    if (pin_qual & 1<<pin_qual_debounced) {
        unsigned int min, max, sum, i;
        min = ~0;
        max = 0;
        sum = 0;

        // Need at least 3 samples to exclude min and max.
        assert(adc_num_debounce_history > 2);

        for (i = 0; i < adc_num_debounce_history; i++) {
            min = MIN(min, adc_debounce[offset][i]);
            max = MAX(max, adc_debounce[offset][i]);
            sum += adc_debounce[offset][i];
        }
        value = (sum-min-max)/(adc_num_debounce_history-2);
    } else {
        value = adc_result[offset];
    }

    value = (uint32)value*pin_analog/4096;

    return value;
}

// this function initializes the adc module.
void
adc_initialize(void)
{
#if ! STICK_GUEST
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    // initialize adc to read all channels
    MCF_ADC_CTRL1 = MCF_ADC_CTRL1_SMODE(0);  // once sequential
    MCF_ADC_CTRL2 = 0x0005;  // divisor for 48 MHz
    MCF_ADC_ADLST1 = 0x3210;
    MCF_ADC_ADLST2 = 0x7654;
    MCF_ADC_ADSDIS = 0x00;
    MCF_ADC_POWER = MCF_ADC_POWER_PUDELAY(13);  // enable adc

#if ! STICKOS
    // AN is primary
    MCF_GPIO_PANPAR = 0xff;
#endif

    delay(10);

    // start the adc
    MCF_ADC_CTRL1 = MCF_ADC_CTRL1_START0;

    delay(10);
#elif MCF51JM128 || MCF51CN128 || MCF51QE128 || MC9S08QE128
    // initialize adc to read one channel at a time
    ADCCFG = ADCCFG_ADLPC_MASK|ADCCFG_ADIV_MASK|ADCCFG_ADLSMP_MASK|ADCCFG_MODE0_MASK|ADCCFG_ADICLK0_MASK;
#elif MC9S12DT256 || MC9S12DP512
    // initialize adc to read all channels
    /* ATD0CTL4: SRES8=0,SMP1=1,SMP0=1,PRS4=0,PRS3=1,PRS2=0,PRS1=1,PRS0=1 */
    ATD0CTL4 = 107;                      /* Set resolution, sample time and prescaler */
    ATD1CTL4 = 107;                      /* Set resolution, sample time and prescaler */
    /* ATD0CTL3: ??=0,S8C=1,S4C=0,S2C=0,S1C=0,FIFO=0,FRZ1=0,FRZ0=0 */
    ATD0CTL3 = 64;                       /* Set ATD control register 3 */
    ATD1CTL3 = 64;                       /* Set ATD control register 3 */
    /* ATD0CTL2: ADPU=1,AFFC=1,AWAI=0,ETRIGLE=0,ETRIGP=0,ETRIGE=0,ASCIE=0,ASCIF=0 */
    ATD0CTL2 = 192;                      /* Set ATD control register 2 */
    ATD1CTL2 = 192;                      /* Set ATD control register 2 */
#elif PIC32
    // initialize adc to read all channels
    AD1CON1 = _AD1CON1_SSRC_MASK|_AD1CON1_ASAM_MASK;
    AD1CON2 = _AD1CON2_CSCNA_MASK|_AD1CON2_SMPI_MASK;
    assert(bus_frequency == 40000000);
    AD1CON3 = (2<<_AD1CON3_SAMC_POSITION)|2;  // assumes 40 MHz; 2 uS

    AD1CSSL = 0xffff;  // sample all 16 inputs
    AD1CON1 |= _AD1CON1_ON_MASK;
#else
#error
#endif
#endif // ! STICK_GUEST
    
    adc_initialized = true;
}

