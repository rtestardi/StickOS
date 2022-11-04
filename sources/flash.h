// *** flash.h ********************************************************

#if MC9S08QE128 || MC9S12DT256 || MC9S12DP512
#pragma CODE_SEG __NEAR_SEG NON_BANKED
#endif
void
flash_erase_pages(uint32 *addr, uint32 npages);

void
flash_write_words(uint32 *addr, uint32 *data, uint32 nwords);
#if MC9S08QE128 || MC9S12DT256 || MC9S12DP512
#pragma CODE_SEG DEFAULT
#endif

void
flash_upgrade(uint32 fsys_frequency);

void
flash_initialize(void);

