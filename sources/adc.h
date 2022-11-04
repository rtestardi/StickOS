// *** adc.h **********************************************************

enum {
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    adc_num_channel = 8
#elif MCF51JM128 || MCF51CN128
    adc_num_channel = 12
#elif MCF51QE128 || MC9S08QE128
    adc_num_channel = 20
#elif MC9S12DT256 || MC9S12DP512
    adc_num_channel = 16
#elif PIC32
    adc_num_channel = 16
#else
#error
#endif
};

#if PICTOCRYPT
extern volatile uint16 adc_result[adc_num_channel];
#endif

void
adc_timer_poll(bool debouncing);

void
adc_sleep();

int
adc_get_value(int offset, int pin_qual);

void
adc_initialize(void);

