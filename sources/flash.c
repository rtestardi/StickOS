// *** flash.c ********************************************************
// this file implements the low level flash control and access, as well
// as the "s19 upgrade" or "hex upgrade" functionality for upgrading
// firmware.

#include "main.h"

#if MCF52221 || MCF52233 || MCF52259 || MCF5211
static
void
flash_command(uint8 cmd, uint32 *addr, uint32 data)
{
    uint32 *backdoor_addr;

    // assert we're initialized
    assert(MCF_CFM_CFMCLKD & MCF_CFM_CFMCLKD_DIVLD);

    // assert we're ready
    assert(MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_CBEIF);

    // assert no errors
    assert(! (MCF_CFM_CFMUSTAT & (MCF_CFM_CFMUSTAT_PVIOL|MCF_CFM_CFMUSTAT_ACCERR)));

    // write the flash thru the backdoor address
    backdoor_addr = (uint32 *)(__IPSBAR+0x04000000+(int)addr);
    *backdoor_addr = data;

    // write the command
    MCF_CFM_CFMCMD = cmd;

    // launch the command (N.B. this clears CBEIF!)
    MCF_CFM_CFMUSTAT = MCF_CFM_CFMUSTAT_CBEIF;

    // busy wait for command buffer empty
    while (! (MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_CBEIF)) {
        // assert no errors
        assert(! (MCF_CFM_CFMUSTAT & (MCF_CFM_CFMUSTAT_PVIOL|MCF_CFM_CFMUSTAT_ACCERR)));
        // NULL
    }

    // assert no errors
    assert(! (MCF_CFM_CFMUSTAT & (MCF_CFM_CFMUSTAT_PVIOL|MCF_CFM_CFMUSTAT_ACCERR)));
}

// this function erases the specified pages of flash memory.
void
flash_erase_pages(uint32 *addr_in, uint32 npages_in)
{
#if SODEBUG
    int i;
#endif
    int x;
    uint32 *addr;
    uint32 npages;

    addr = addr_in;
    npages = npages_in;
    
    x = splx(7);
    
    // while there are more pages to erase...
    while (npages) {
        flash_command(MCF_CFM_CFMCMD_PAGE_ERASE, addr, 0);
        npages--;
        addr += FLASH_PAGE_SIZE/sizeof(uint32);
    }

    // busy wait for flash command complete
    while (! (MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_CCIF)) {
        // NULL
    }
    
    (void)splx(x);

#if SODEBUG
    for (i = 0; i < npages_in*FLASH_PAGE_SIZE/sizeof(uint32); i++) {
        assert(addr_in[i] == -1);
    }
#endif
}

// this function writes the specified words of flash memory.
void
flash_write_words(uint32 *addr_in, uint32 *data_in, uint32 nwords_in)
{
#if SODEBUG
    int i;
#endif
    int x;
    uint32 *addr;
    uint32 *data;
    uint32 nwords;

    addr = addr_in;
    data = data_in;
    nwords = nwords_in;
    
    x = splx(7);

    // while there are more words to program...
    while (nwords) {
        flash_command(MCF_CFM_CFMCMD_WORD_PROGRAM, addr, *data);
        nwords--;
        addr++;
        data++;
    }

    // busy wait for flash command complete
    while (! (MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_CCIF)) {
        // NULL
    }
    
    (void)splx(x);

#if SODEBUG
    for (i = 0; i < nwords_in; i++) {
        assert(addr_in[i] == data_in[i]);
    }
#endif
}
#elif MCF51JM128 || MCF51CN128 || MCF51QE128 || MC9S08QE128 || MC9S12DT256 || MC9S12DP512
#if MC9S08QE128 || MC9S12DT256 || MC9S12DP512
#pragma CODE_SEG __NEAR_SEG NON_BANKED
#else
#define near
#endif
static
void
flash_command_ram(uint8 cmd, uint32 *addr, uint32 data)
{
    // N.B. this code generates no relocations so we can run it from RAM!!!

    // write the flash thru the frontdoor address
#if MC9S08QE128
    *(byte *)addr = data;
#elif MC9S12DT256 || MC9S12DP512
    *(uint16 *)addr = data;
#else
    *addr = data;
#endif

    // write the command
    MCF_CFM_CFMCMD = cmd;

    // launch the command (N.B. this clears CBEIF!)
    MCF_CFM_CFMUSTAT = MCF_CFM_CFMUSTAT_CBEIF;

    // busy wait for flash command complete
    while (! (MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_CCIF)) {
        // assert no errors
        assert_ram(! (MCF_CFM_CFMUSTAT & (MCF_CFM_CFMUSTAT_PVIOL|MCF_CFM_CFMUSTAT_ACCERR)));
        // NULL
    }

    // assert no errors
    assert_ram(! (MCF_CFM_CFMUSTAT & (MCF_CFM_CFMUSTAT_PVIOL|MCF_CFM_CFMUSTAT_ACCERR)));
}

static
void
flash_command(uint8 cmd, uint32 *addr, uint32 data)
{
    void *fn;
    
    // assert we're initialized
    assert(MCF_CFM_CFMCLKD & MCF_CFM_CFMCLKD_DIVLD);

    // assert we're ready
    assert(MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_CBEIF);
    
    // assert no errors
    MCF_CFM_CFMUSTAT |= MCF_CFM_CFMUSTAT_PVIOL|MCF_CFM_CFMUSTAT_ACCERR;
    assert(! (MCF_CFM_CFMUSTAT & (MCF_CFM_CFMUSTAT_PVIOL|MCF_CFM_CFMUSTAT_ACCERR)));
    
    fn = (void *)(((uint32)big_buffer+3)&~3);
    assert((uint32)flash_command-(uint32)flash_command_ram <= sizeof(big_buffer));
    memcpy(fn, flash_command_ram, (uint32)flash_command-(uint32)flash_command_ram);
    ((void (* near)(uint8, uint32 *, uint32))fn)(cmd, addr, data);
}

// this function erases the specified pages of flash memory.
void
flash_erase_pages(uint32 *addr_in, uint32 npages_in)
{
#if SODEBUG
    int i;
#endif
    int x;
    uint32 *addr;
    uint32 npages;

    addr = addr_in;
    npages = npages_in;
        
    x = splx(7);
    
    // while there are more pages to erase...
    while (npages) {
        flash_command(MCF_CFM_CFMCMD_PAGE_ERASE, addr, 0);
        npages--;
        addr += FLASH_PAGE_SIZE/sizeof(uint32);
    }

    (void)splx(x);

#if SODEBUG
    for (i = 0; i < npages_in*FLASH_PAGE_SIZE/sizeof(uint32); i++) {
        assert(addr_in[i] == -1);
    }
#endif
}

// this function writes the specified words of flash memory.
void
flash_write_words(uint32 *addr_in, uint32 *data_in, uint32 nwords_in)
{
#if SODEBUG
    int i;
#endif
#if MC9S08QE128
    int x;
    byte *addr;
    byte *data;
    uint32 nbytes;

    addr = (byte *)addr_in;
    data = (byte *)data_in;
    nbytes = nwords_in*sizeof(uint32);
        
    x = splx(7);

    // while there are more bytes to program...
    while (nbytes) {
        flash_command(MCF_CFM_CFMCMD_WORD_PROGRAM, (uint32 *)addr, (uint32)*data);
        nbytes--;
        addr++;
        data++;
    }

    (void)splx(x);
#elif MC9S12DT256 || MC9S12DP512
    int x;
    uint16 *addr;
    uint16 *data;
    uint32 nshorts;

    addr = (uint16 *)addr_in;
    data = (uint16 *)data_in;
    nshorts = nwords_in*sizeof(uint16);
        
    x = splx(7);

    // while there are more shorts to program...
    while (nshorts) {
        flash_command(MCF_CFM_CFMCMD_WORD_PROGRAM, (uint32 *)addr, (uint32)*data);
        nshorts--;
        addr++;
        data++;
    }

    (void)splx(x);
#else
    int x;
    uint32 *addr;
    uint32 *data;
    uint32 nwords;

    addr = addr_in;
    data = data_in;
    nwords = nwords_in;
        
    x = splx(7);

    // while there are more words to program...
    while (nwords) {
        flash_command(MCF_CFM_CFMCMD_WORD_PROGRAM, addr, *data);
        nwords--;
        addr++;
        data++;
    }

    (void)splx(x);
#endif

#if SODEBUG
    for (i = 0; i < nwords_in; i++) {
        assert(addr_in[i] == data_in[i]);
    }
#endif
}
#if MC9S08QE128 || MC9S12DT256 || MC9S12DP512
#pragma CODE_SEG DEFAULT
#endif

#elif PIC32
#define NVMOP_PAGE_ERASE        0x4004      // Page erase operation
#define NVMOP_WORD_PGM          0x4001      // Word program operation

static
void
__attribute__((nomips16))
flash_operation(unsigned int nvmop)
{
    // Enable Flash Write/Erase Operations
    NVMCON = NVMCON_WREN | nvmop;

    // wait at least 6 us for LVD start-up
    // assume we're running at max frequency
    // (80 MHz) so we're always safe
    {
        unsigned long t0 = _CP0_GET_COUNT();
        while (_CP0_GET_COUNT() - t0 < (80/2)*6);
    }

    NVMKEY = 0xAA996655;
    NVMKEY = 0x556699AA;
    NVMCONSET = NVMCON_WR;

    // Wait for WR bit to clear
    while (NVMCON & NVMCON_WR) {
        // NULL
    }
    assert_ram(! (NVMCON & NVMCON_WR));

    // Disable Flash Write/Erase operations
    NVMCONCLR = NVMCON_WREN;

    // assert no errors
    assert_ram(! (NVMCON & (_NVMCON_WRERR_MASK | _NVMCON_LVDERR_MASK)));
}

void
flash_erase_pages(uint32 *addr_in, uint32 npages_in)
{
#if SODEBUG
    int i;
#endif
    int x;
    uint32 *addr;
    uint32 npages;

    addr = addr_in;
    npages = npages_in;
        
    x = splx(7);
#if defined(USBOTG)
    DMACONSET = _DMACON_SUSPEND_MASK;
    while (! DMACONbits.SUSPEND) {
        // NULL
    }
#endif
    
    // while there are more pages to erase...
    while (npages) {
        // Convert Address to Physical Address
        NVMADDR = KVA_TO_PA((unsigned int)addr);
    
        // Unlock and Erase Page
        flash_operation(NVMOP_PAGE_ERASE);

        npages--;
        addr += FLASH_PAGE_SIZE/sizeof(uint32);
    }

#if defined(USBOTG)
    DMACONCLR = _DMACON_SUSPEND_MASK;
#endif
    (void)splx(x);

#if SODEBUG
    for (i = 0; i < npages_in*FLASH_PAGE_SIZE/sizeof(uint32); i++) {
        assert(addr_in[i] == -1);
    }
#endif
}

void
flash_write_words(uint32 *addr_in, uint32 *data_in, uint32 nwords_in)
{
#if SODEBUG
    int i;
#endif
    int x;
    uint32 *addr;
    uint32 *data;
    uint32 nwords;

    addr = addr_in;
    data = data_in;
    nwords = nwords_in;

    x = splx(7);
#if defined(USBOTG)
    DMACONSET = _DMACON_SUSPEND_MASK;
    while (! DMACONbits.SUSPEND) {
        // NULL
    }
#endif
    
    while (nwords--) {
        // Convert Address to Physical Address
        NVMADDR = KVA_TO_PA((unsigned int)addr);
    
        // Load data into NVMDATA register
        NVMDATA = *data;
    
        // Unlock and Write Word
        flash_operation(NVMOP_WORD_PGM);
    
        addr++;
        data++;
    }

#if defined(USBOTG)
    DMACONCLR = _DMACON_SUSPEND_MASK;
#endif
    (void)splx(x);

#if SODEBUG
    for (i = 0; i < nwords_in; i++) {
        assert(addr_in[i] == data_in[i]);
    }
#endif
}
#else
#error
#endif

#if UPGRADE
// this function performs the final step of a firmware flash upgrade.
FLASH_UPGRADE_RAM_BEGIN
void
#if PIC32
__longramfunc__
__attribute__((nomips16))
#endif
flash_upgrade_ram_begin(void)
{
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    uint32 *addr;
    uint32 *data;
    uint32 nwords;
    uint32 npages;
    uint32 *backdoor_addr;

    // N.B. this code generates no relocations so we can run it from RAM!!!

    // erase the firmware code
    // flash_erase_pages()
    addr = NULL;
    npages = FLASH_BYTES/2/FLASH_PAGE_SIZE;
    while (npages) {
        assert_ram(MCF_CFM_CFMCLKD & MCF_CFM_CFMCLKD_DIVLD);
        assert_ram(MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_CBEIF);
        assert_ram(! (MCF_CFM_CFMUSTAT & (MCF_CFM_CFMUSTAT_PVIOL|MCF_CFM_CFMUSTAT_ACCERR)));
        backdoor_addr = (uint32 *)(__IPSBAR+0x04000000+(int)addr);
        *backdoor_addr = 0;
        MCF_CFM_CFMCMD = MCF_CFM_CFMCMD_PAGE_ERASE;
        MCF_CFM_CFMUSTAT = MCF_CFM_CFMUSTAT_CBEIF;
        while (! (MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_CBEIF)) {
            assert_ram(! (MCF_CFM_CFMUSTAT & (MCF_CFM_CFMUSTAT_PVIOL|MCF_CFM_CFMUSTAT_ACCERR)));
        }
        assert_ram(! (MCF_CFM_CFMUSTAT & (MCF_CFM_CFMUSTAT_PVIOL|MCF_CFM_CFMUSTAT_ACCERR)));
        npages--;
        addr += FLASH_PAGE_SIZE/sizeof(uint32);
    }
    while (! (MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_CCIF)) {
    }

    // and re-flash it from the staging area
    // flash_write_words()
    addr = NULL;
    data = (uint32 *)(FLASH_BYTES/2);
    nwords = FLASH_BYTES/2/sizeof(uint32);
    while (nwords) {
        assert_ram(MCF_CFM_CFMCLKD & MCF_CFM_CFMCLKD_DIVLD);
        assert_ram(MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_CBEIF);
        assert_ram(! (MCF_CFM_CFMUSTAT & (MCF_CFM_CFMUSTAT_PVIOL|MCF_CFM_CFMUSTAT_ACCERR)));
        backdoor_addr = (uint32 *)(__IPSBAR+0x04000000+(int)addr);
        *backdoor_addr = *data;
        MCF_CFM_CFMCMD = MCF_CFM_CFMCMD_WORD_PROGRAM;
        MCF_CFM_CFMUSTAT = MCF_CFM_CFMUSTAT_CBEIF;
        while (! (MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_CBEIF)) {
            assert_ram(! (MCF_CFM_CFMUSTAT & (MCF_CFM_CFMUSTAT_PVIOL|MCF_CFM_CFMUSTAT_ACCERR)));
        }
        assert_ram(! (MCF_CFM_CFMUSTAT & (MCF_CFM_CFMUSTAT_PVIOL|MCF_CFM_CFMUSTAT_ACCERR)));
        ASSERT_RAM(*addr == *data);
        nwords--;
        addr++;
        data++;
    }
    while (! (MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_CCIF)) {
    }

    // erase the staging area
    // flash_erase_pages(FLASH_BYTES/2, FLASH_BYTES/2/FLASH_PAGE_SIZE)
    addr = (uint32 *)(FLASH_BYTES/2);
    npages = FLASH_BYTES/2/FLASH_PAGE_SIZE;
    while (npages) {
        assert_ram(MCF_CFM_CFMCLKD & MCF_CFM_CFMCLKD_DIVLD);
        assert_ram(MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_CBEIF);
        assert_ram(! (MCF_CFM_CFMUSTAT & (MCF_CFM_CFMUSTAT_PVIOL|MCF_CFM_CFMUSTAT_ACCERR)));
        backdoor_addr = (uint32 *)(__IPSBAR+0x04000000+(int)addr);
        *backdoor_addr = 0;
        MCF_CFM_CFMCMD = MCF_CFM_CFMCMD_PAGE_ERASE;
        MCF_CFM_CFMUSTAT = MCF_CFM_CFMUSTAT_CBEIF;
        while (! (MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_CBEIF)) {
            assert_ram(! (MCF_CFM_CFMUSTAT & (MCF_CFM_CFMUSTAT_PVIOL|MCF_CFM_CFMUSTAT_ACCERR)));
        }
        assert_ram(! (MCF_CFM_CFMUSTAT & (MCF_CFM_CFMUSTAT_PVIOL|MCF_CFM_CFMUSTAT_ACCERR)));
        npages--;
        addr += FLASH_PAGE_SIZE/sizeof(uint32);
    }
    while (! (MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_CCIF)) {
    }

    // reset the MCU
    MCF_RCM_RCR = MCF_RCM_RCR_SOFTRST;
    asm_halt();
#elif MCF51JM128 || MCF51CN128 || MCF51QE128
    uint32 *addr;
    uint32 *data;
    uint32 nwords;
    uint32 npages;

    // N.B. this code generates no relocations so we can run it from RAM!!!

    // erase the firmware code
    // flash_erase_pages()
    addr = NULL;
    npages = FLASH_BYTES/2/FLASH_PAGE_SIZE;
    while (npages) {
        assert_ram(MCF_CFM_CFMCLKD & MCF_CFM_CFMCLKD_DIVLD);
        assert_ram(MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_CBEIF);
        MCF_CFM_CFMUSTAT |= MCF_CFM_CFMUSTAT_PVIOL|MCF_CFM_CFMUSTAT_ACCERR;
        assert_ram(! (MCF_CFM_CFMUSTAT & (MCF_CFM_CFMUSTAT_PVIOL|MCF_CFM_CFMUSTAT_ACCERR)));
        *addr = 0;
        MCF_CFM_CFMCMD = MCF_CFM_CFMCMD_PAGE_ERASE;
        MCF_CFM_CFMUSTAT = MCF_CFM_CFMUSTAT_CBEIF;
        while (! (MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_CCIF)) {
        }
        assert_ram(! (MCF_CFM_CFMUSTAT & (MCF_CFM_CFMUSTAT_PVIOL|MCF_CFM_CFMUSTAT_ACCERR)));
        npages--;
        addr += FLASH_PAGE_SIZE/sizeof(uint32);
    }
    while (! (MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_CCIF)) {
    }

    // and re-flash it from the staging area
    // flash_write_words()
    addr = NULL;
    data = (uint32 *)(FLASH_BYTES/2);
    nwords = FLASH_BYTES/2/sizeof(uint32);
    while (nwords) {
        assert_ram(MCF_CFM_CFMCLKD & MCF_CFM_CFMCLKD_DIVLD);
        assert_ram(MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_CBEIF);
        MCF_CFM_CFMUSTAT |= MCF_CFM_CFMUSTAT_PVIOL|MCF_CFM_CFMUSTAT_ACCERR;
        assert_ram(! (MCF_CFM_CFMUSTAT & (MCF_CFM_CFMUSTAT_PVIOL|MCF_CFM_CFMUSTAT_ACCERR)));
        *addr = *data;
        MCF_CFM_CFMCMD = MCF_CFM_CFMCMD_WORD_PROGRAM;
        MCF_CFM_CFMUSTAT = MCF_CFM_CFMUSTAT_CBEIF;
        while (! (MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_CCIF)) {
        }
        assert_ram(! (MCF_CFM_CFMUSTAT & (MCF_CFM_CFMUSTAT_PVIOL|MCF_CFM_CFMUSTAT_ACCERR)));
        ASSERT_RAM(*addr == *data);
        nwords--;
        addr++;
        data++;
    }
    while (! (MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_CCIF)) {
    }

    // erase the staging area
    // flash_erase_pages(FLASH_BYTES/2, FLASH_BYTES/2/FLASH_PAGE_SIZE)
    addr = (uint32 *)(FLASH_BYTES/2);
    npages = FLASH_BYTES/2/FLASH_PAGE_SIZE;
    while (npages) {
        assert_ram(MCF_CFM_CFMCLKD & MCF_CFM_CFMCLKD_DIVLD);
        assert_ram(MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_CBEIF);
        MCF_CFM_CFMUSTAT |= MCF_CFM_CFMUSTAT_PVIOL|MCF_CFM_CFMUSTAT_ACCERR;
        assert_ram(! (MCF_CFM_CFMUSTAT & (MCF_CFM_CFMUSTAT_PVIOL|MCF_CFM_CFMUSTAT_ACCERR)));
        *addr = 0;
        MCF_CFM_CFMCMD = MCF_CFM_CFMCMD_PAGE_ERASE;
        MCF_CFM_CFMUSTAT = MCF_CFM_CFMUSTAT_CBEIF;
        while (! (MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_CCIF)) {
        }
        assert_ram(! (MCF_CFM_CFMUSTAT & (MCF_CFM_CFMUSTAT_PVIOL|MCF_CFM_CFMUSTAT_ACCERR)));
        npages--;
        addr += FLASH_PAGE_SIZE/sizeof(uint32);
    }
    while (! (MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_CCIF)) {
    }

    // reset the MCU
    asm {
        move.l  #0x00000000,d0
        movec   d0,CPUCR
        trap    #0
    };
    asm_halt();
#elif MC9S08QE128 || MC9S12DT256 || MC9S12DP512
    asm_halt();
#elif PIC32
    uint32 *addr;
    uint32 *data;
    uint32 nwords;
    uint32 npages;

    // N.B. this code generates no relocations so we can run it from RAM!!!

    // erase the program flash
    // flash_erase_pages()
    addr = (uint32 *)FLASH_START;
    npages = FLASH_BYTES/2/FLASH_PAGE_SIZE;
    while (npages) {
        NVMADDR = KVA_TO_PA((unsigned int)addr);
        NVMCON = NVMCON_WREN | NVMOP_PAGE_ERASE;
        NVMKEY = 0xAA996655;
        NVMKEY = 0x556699AA;
        NVMCONSET = NVMCON_WR;
        while (NVMCON & NVMCON_WR) {
        }
        assert_ram(! (NVMCON & NVMCON_WR));
        NVMCONCLR = NVMCON_WREN;
        assert_ram(! (NVMCON & (_NVMCON_WRERR_MASK | _NVMCON_LVDERR_MASK)));
        npages--;
        addr += FLASH_PAGE_SIZE/sizeof(uint32);
    }

    // and re-flash the program flash from the staging area
    // flash_write_words()
    addr = (uint32 *)FLASH_START;
    data = (uint32 *)(FLASH_START+FLASH_BYTES/2);
    nwords = (FLASH_BYTES/2 - FLASH2_BYTES)/sizeof(uint32);
    while (nwords) {
        NVMADDR = KVA_TO_PA((unsigned int)addr);
        NVMDATA = *data;
        NVMCON = NVMCON_WREN | NVMOP_WORD_PGM;
        NVMKEY = 0xAA996655;
        NVMKEY = 0x556699AA;
        NVMCONSET = NVMCON_WR;
        while (NVMCON & NVMCON_WR) {
        }
        assert_ram(! (NVMCON & NVMCON_WR));
        NVMCONCLR = NVMCON_WREN;
        assert_ram(! (NVMCON & (_NVMCON_WRERR_MASK | _NVMCON_LVDERR_MASK)));
        ASSERT_RAM(*addr == *data);
        nwords--;
        addr++;
        data++;
    }

    // erase the boot flash
    // flash_erase_pages()
    addr = (uint32 *)FLASH2_START;
    npages = FLASH2_BYTES/FLASH_PAGE_SIZE;
    while (npages) {
        NVMADDR = KVA_TO_PA((unsigned int)addr);
        NVMCON = NVMCON_WREN | NVMOP_PAGE_ERASE;
        NVMKEY = 0xAA996655;
        NVMKEY = 0x556699AA;
        NVMCONSET = NVMCON_WR;
        while (NVMCON & NVMCON_WR) {
        }
        assert_ram(! (NVMCON & NVMCON_WR));
        NVMCONCLR = NVMCON_WREN;
        assert_ram(! (NVMCON & (_NVMCON_WRERR_MASK | _NVMCON_LVDERR_MASK)));
        npages--;
        addr += FLASH_PAGE_SIZE/sizeof(uint32);
    }

    // and re-flash the boot flash from the staging area
    // flash_write_words()
    addr = (uint32 *)FLASH2_START;
    data = (uint32 *)(FLASH_START+FLASH_BYTES-FLASH2_BYTES);
    nwords = FLASH2_BYTES/sizeof(uint32);
    while (nwords) {
        NVMADDR = KVA_TO_PA((unsigned int)addr);
        NVMDATA = *data;
        NVMCON = NVMCON_WREN | NVMOP_WORD_PGM;
        NVMKEY = 0xAA996655;
        NVMKEY = 0x556699AA;
        NVMCONSET = NVMCON_WR;
        while (NVMCON & NVMCON_WR) {
        }
        assert_ram(! (NVMCON & NVMCON_WR));
        NVMCONCLR = NVMCON_WREN;
        assert_ram(! (NVMCON & (_NVMCON_WRERR_MASK | _NVMCON_LVDERR_MASK)));
        ASSERT_RAM(*addr == *data);
        nwords--;
        addr++;
        data++;
    }

    // erase the staging area
    // flash_erase_pages(FLASH_BYTES/2, FLASH_BYTES/2/FLASH_PAGE_SIZE)
    addr = (uint32 *)(FLASH_START+FLASH_BYTES/2);
    npages = FLASH_BYTES/2/FLASH_PAGE_SIZE;
    while (npages) {
        NVMADDR = KVA_TO_PA((unsigned int)addr);
        NVMCON = NVMCON_WREN | NVMOP_PAGE_ERASE;
        NVMKEY = 0xAA996655;
        NVMKEY = 0x556699AA;
        NVMCONSET = NVMCON_WR;
        while (NVMCON & NVMCON_WR) {
        }
        assert_ram(! (NVMCON & NVMCON_WR));
        NVMCONCLR = NVMCON_WREN;
        assert_ram(! (NVMCON & (_NVMCON_WRERR_MASK | _NVMCON_LVDERR_MASK)));
        npages--;
        addr += FLASH_PAGE_SIZE/sizeof(uint32);
    }

    // reset the MCU
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
}

// this function just demarcates the end of flash_upgrade_ram_begin().
FLASH_UPGRADE_RAM_END
void
flash_upgrade_ram_end(void)
{
    // NULL
}
#endif

#if DOWNLOAD || UPGRADE
// this function downloads a new s19 firmware file to a staging
// area, and then installs it by calling a RAM copy of
// flash_upgrade_ram_begin().
void
flash_upgrade(uint32 fsys_frequency)
{
#if MCF52221 || MCF52233 || MCF52259 || MCF5211 || MCF51JM128 || MCF51CN128 || MCF51QE128
    int i;
    int n;
#if UPGRADE
    int x;
#endif
    int y;
    char c;
    int sum;
    int addr;
    int count;
    char *s19;
    bool done;
    bool error;
    uint32 data;
    bool begun;
#if UPGRADE
    flash_upgrade_ram_begin_f fn;
#endif

    if (fsys_frequency) {
#if ! PICTOCRYPT
        // we are relaying S19 to QSPI to EzPort
        clone_init(fsys_frequency);
#endif
    } else {
        if ((int)end_of_static > FLASH_BYTES/2) {
            printf("code exceeds half of flash\n");
            return;
        }

        // erase the staging area
        flash_erase_pages((uint32 *)(FLASH_BYTES/2), FLASH_BYTES/2/FLASH_PAGE_SIZE);
    }

    printf("paste S19 upgrade file now...\n");
    terminal_echo = false;

    y = 0;
    done = false;
    error = false;
    begun = false;

    do {
        // wait for an s19 command line
        if (main_command) {
            main_command = NULL;
            terminal_command_ack(false);
        }

        while (! main_command) {
            terminal_poll();
        }

        s19 = main_command;
        while (isspace(*s19)) {
            s19++;
        }

        if (! *s19) {
            continue;
        }

        sum = 0;

        // parse s19 header
        if (*s19++ != 'S') {
            printf("\nbad record\n");
            break;
        }
        c = *s19++;

        if (c == '0') {
            begun = true;
        } else if ((c == '1') || (c == '3')) {
            // parse data record
            const int addr_len = (c == '1') ? 2 : 4;

            // 1 byte of count
            n = get2hex(&s19);
            if (n == -1) {
                printf("\nbad count\n");
                break;
            }
            sum += n;
            count = n;
            // we flash 4 bytes at a time!  Remove checksum byte and
            // address bytes.
            if ((count-1-addr_len) % 4) {
                printf("\nbad count\n");
                break;
            }

            // 2 or 4 bytes of address
            addr = 0;
            for (i = 0; i < addr_len; i++) {
                n = get2hex(&s19);
                if (n == -1) {
                    printf("\nbad address\n");
                    break;
                }
                sum += n;
                addr = addr*256+n;
            }
            if (i != addr_len) {
                break;
            }

            assert(count > addr_len);
            count -= addr_len;

            // while there is more data
            while (count > 1) {
                assert((count-1) % 4 == 0);

                // get 4 bytes of data
                data = 0;
                for (i = 0; i < 4; i++) {
                    n = get2hex(&s19);
                    if (n == -1) {
                        printf("\nbad data\n");
                        break;
                    }
                    sum += n;
                    data = data*256+n;  // endian
                }
                if (i != 4) {
                    break;
                }

                if (fsys_frequency) {
#if ! PICTOCRYPT
                    // we are relaying S19 to QSPI to EzPort
                    if (! clone_program(addr, &data, sizeof(data))) {
                        error = true;
                    }
#endif
                } else {
                    if ((int)addr < FLASH_BYTES/2) {
                        if (*(uint32 *)(FLASH_BYTES/2+addr) == -1) {
                            // program the words
                            flash_write_words((uint32 *)(FLASH_BYTES/2+addr), &data, 1);
                        } else if (! error) {
                            printf("\nduplicate address 0x%x\n", addr);
                            error = true;
                        }
                    } else if (! error) {
                        printf("\nbad address 0x%x\n", addr);
                        error = true;
                    }
                }

                addr += 4;
                assert(count > 4);
                count -= 4;
            }
            if (count > 1) {
                break;
            }
            
            // verify 1 byte of checksum
            assert(count == 1);
            n = get2hex(&s19);
            if (n == -1) {
                printf("\nbad checksum\n");
                break;
            }
            sum += n;
            if ((sum & 0xff) != 0xff) {
                printf("\nbad checksum 0x%x\n", sum & 0xff);
                break;
            }

            if (++y%4 == 0) {
                printf(".");
#if MCF52233
            } else if (y%2) {
                // fec gets confused with interrupts off and no
                // characters in flight!
                printf("%c", '\0');
#endif
            }
        } else if ((c == '7') || (c == '8') || (c == '9')) {
            done = true;
        } else {
            // unrecognized record
            break;
        }
    } while (! done);

    if (! begun || ! done || error) {
        if (main_command) {
            main_command = NULL;
            terminal_command_ack(false);
        }

        // we're in trouble!
        if (! begun) {
            printf("s0 record not found\n");
        }
        if (! done) {
            printf("s7/8/9 record not found\n");
        }
        printf("upgrade failed\n");
        terminal_echo = true;

#if STICKOS
        // erase the staging area
        code_new();
#endif
        return;
    }

    // we're committed to upgrade!
    printf("\npaste done!\n");
    
    if (fsys_frequency) {
#if ! PICTOCRYPT
        clone_reset();
#endif
    
        if (main_command) {
            main_command = NULL;
            terminal_command_ack(false);
        }

        terminal_echo = true;
        return;
    }
#if UPGRADE
    printf("programming flash for upgrade...\n");
    printf("wait for heartbeat LED to blink!\n");
    delay(100);

    // N.B. this is an incompatible upgrade; if we crash before we are through,
    // we might be in trouble.
    ASSERT((int)flash_upgrade_ram_end - (int)flash_upgrade_ram_begin <= sizeof(big_buffer)-4);
    
    // disable interrupts
    x = splx(7);
    
    // copy our old flash upgrade routine to RAM
    fn = (flash_upgrade_ram_begin_f)(((uint32)big_buffer+3)&~3);
    memcpy(fn, (void *)flash_upgrade_ram_begin, (int)flash_upgrade_ram_end - (int)flash_upgrade_ram_begin);

    // and run it!
    fn();
#endif

    // we should not come back!
    ASSERT(0);  // stop!
#elif MC9S08QE128 || MC9S12DT256 || MC9S12DP512
#elif PIC32
    int i;
    int n;
    int x;
    int y;
    char c;
    int sum;
    int addr;
    int paddr;
    int vaddr;
    int taddr;
    int type;
    int count;
    int extend;
    char *hex;
    bool done;
    bool error;
    uint32 data;
    uint32 zero;
    bool begun;

    if ((int)end_of_static > FLASH_START+FLASH_BYTES/2-FLASH2_BYTES) {
        printf("code exceeds half of flash\n");
        return;
    }

    // erase the staging area
    flash_erase_pages((uint32 *)(FLASH_START+FLASH_BYTES/2), FLASH_BYTES/2/FLASH_PAGE_SIZE);

    printf("paste HEX upgrade file now...\n");
    terminal_echo = false;

    y = 0;
    done = false;
    error = false;
    begun = false;

    do {
        // wait for an hex command line
        if (main_command) {
            main_command = NULL;
            terminal_command_ack(false);
        }

        while (! main_command) {
            terminal_poll();
        }

        hex = main_command;
        while (isspace(*hex)) {
            hex++;
        }

        if (! *hex) {
            continue;
        }

        sum = 0;

        // parse hex header
        if (*hex++ != ':') {
            printf("\nbad record\n");
            break;
        }

        // 1 byte of count
        n = get2hex(&hex);
        if (n == -1) {
            printf("\nbad count\n");
            break;
        }
        sum += n;
        count = n;

        // 2 bytes of address
        addr = 0;
        for (i = 0; i < 2; i++) {
            n = get2hex(&hex);
            if (n == -1) {
                printf("\nbad address\n");
                break;
            }
            sum += n;
            addr = addr*256+n;
        }
        if (i != 2) {
            break;
        }
        addr = extend<<16 | addr;
        paddr = (int)KVA_TO_PA(addr);
        vaddr = (int)PA_TO_KVA1(paddr);

        // 1 byte of record type
        n = get2hex(&hex);
        if (n == -1) {
           printf("\nbad type\n");
            break;
        }
        sum += n;
        type = n;

        if (type == 1 && count == 0) {
            // eof
            done = true;
        } else if (type == 4 && count == 2) {
            // get the extended address
            extend = 0;
            for (i = 0; i < 2; i++) {
                n = get2hex(&hex);
                if (n == -1) {
                    printf("\nbad address\n");
                    break;
                }
                sum += n;
                extend = extend*256+n;
            }
            if (i != 2) {
                break;
            }
            begun = true;
        } else if (type == 0) {
            // we flash 4 bytes at a time!
            if (count % 4) {
                printf("\nbad count\n");
                break;
            }
    
            // while there is more data
            while (count) {
                assert(count % 4 == 0);

                // get 4 bytes of data
                data = 0;
                for (i = 0; i < 4; i++) {
                    n = get2hex(&hex);
                    if (n == -1) {
                        printf("\nbad data\n");
                        break;
                    }
                    sum += n;
                    data = data|(n<<(8*i));  // endian
                }
                if (i != 4) {
                    break;
                }
 
                if (paddr >= KVA_TO_PA(FLASH_START) && paddr < KVA_TO_PA(FLASH_START)+FLASH_BYTES/2-FLASH2_BYTES) {
                    if (*(uint32 *)(vaddr+FLASH_BYTES/2) == -1) {
                        // program the words
                        flash_write_words((uint32 *)(vaddr+FLASH_BYTES/2), &data, 1);
                    } else if (! error) {
                        printf("\nduplicate address 0x%x\n", paddr);
                        error = true;
                    }
                } else if (paddr >= KVA_TO_PA(FLASH2_START) && paddr < KVA_TO_PA(FLASH2_START)+FLASH2_BYTES) {
                    taddr = FLASH_START + FLASH_BYTES - FLASH2_BYTES + (vaddr - FLASH2_START);
                    if (*(uint32 *)taddr == -1) {
                        // this is bootflash data; program the words in the last FLASH2_BYTES of the staging area
                        flash_write_words((uint32 *)taddr, &data, 1);
                    } else if (! error) {
                        printf("\nduplicate address 0x%x\n", paddr);
                        error = true;
                    }
                } else if (! error) {
                    printf("\nbad address 0x%x\n", paddr);
                    error = true;
                }

                addr += 4;
                vaddr += 4;
                paddr += 4;
                assert(count >= 4);
                count -= 4;
            }
            if (count) {
                break;
            }
        } else {
            printf("\nbad record\n");
            break;
        }
 
        // verify 1 byte of checksum
        n = get2hex(&hex);
        if (n == -1) {
            printf("\nbad checksum\n");
            break;
        }
        sum += n;
        if ((sum & 0xff) != 0x00) {
            printf("\nbad checksum 0x%x\n", sum & 0xff);
            break;
        }

        if (++y%4 == 0) {
            printf(".");
        }
    } while (! done);

    if (! begun || ! done || error) {
        if (main_command) {
            main_command = NULL;
            terminal_command_ack(false);
        }

        // we're in trouble!
        if (! begun) {
            printf(":04 record not found\n");
        }
        if (! done) {
            printf(":01 record not found\n");
        }
        printf("upgrade failed\n");
        terminal_echo = true;

#if STICKOS
        // erase the staging area
        code_new();
#endif
        return;
    }

    // we're committed to upgrade!
    printf("\npaste done!\n");
    
    printf("programming flash for upgrade...\n");
    printf("wait for heartbeat LED to blink!\n");
    delay(100);

    // N.B. this is an incompatible upgrade; if we crash before we are through,
    // we might be in trouble.
    
    // disable interrupts
    x = splx(7);
#if defined(USBOTG)
    DMACONSET = _DMACON_SUSPEND_MASK;
    while (! DMACONbits.SUSPEND) {
        // NULL
    }
#endif

    delay(100);
    
    // N.B. the flash upgrade routine is already copied to RAM
    // run it!
    flash_upgrade_ram_begin();

    // we should not come back!
    ASSERT(0);  // stop!
#else
#error
#endif
}
#endif

// this function initializes the flash module.
void
flash_initialize(void)
{
#if UPGRADE
#if ! PIC32
    assert((int)flash_upgrade_ram_end - (int)flash_upgrade_ram_begin <= sizeof(big_buffer));
#endif
#endif

#if MCF52221 || MCF52233 || MCF52259 || MCF5211 || MCF51JM128 || MCF51CN128 || MCF51QE128 || MC9S08QE128 || MC9S12DT256 || MC9S12DP512
#if MC9S12DT256 || MC9S12DP512
    if (oscillator_frequency > 12800000) {
        MCF_CFM_CFMCLKD = MCF_CFM_CFMCLKD_PRDIV8|MCF_CFM_CFMCLKD_DIV((oscillator_frequency/8-1)/200000);
    } else {
        MCF_CFM_CFMCLKD = MCF_CFM_CFMCLKD_DIV((oscillator_frequency-1)/200000);
    }
    
#define setReg8(RegName, val)                                    (RegName = (byte)(val))
    /* FCNFG: CBEIE=0,CCIE=0,KEYACC=0,??=0,??=0,??=0,BKSEL1=0,BKSEL0=1 */
    setReg8(FCNFG, 1);                   /* Select the flash block #1 */ 
    /* FSTAT: CBEIF=0,CCIF=0,PVIOL=1,ACCERR=1,??=0,BLANK=0,??=0,??=0 */
    setReg8(FSTAT, 48);                  /* Clear error flags PVIOL and ACCERR */ 
    /* FPROT: FPOPEN=1,NV6=1,FPHDIS=1,FPHS1=1,FPHS0=1,FPLDIS=1,FPLS1=1,FPLS0=1 */
    setReg8(FPROT, 255);                 /* Set protection of flash block #1 */ 
    /* FCNFG: CBEIE=0,CCIE=0,KEYACC=0,??=0,??=0,??=0,BKSEL1=1,BKSEL0=0 */
    setReg8(FCNFG, 2);                   /* Select the flash block #2 */ 
    /* FSTAT: CBEIF=0,CCIF=0,PVIOL=1,ACCERR=1,??=0,BLANK=0,??=0,??=0 */
    setReg8(FSTAT, 48);                  /* Clear error flags PVIOL and ACCERR */ 
    /* FPROT: FPOPEN=1,NV6=1,FPHDIS=1,FPHS1=1,FPHS0=1,FPLDIS=1,FPLS1=1,FPLS0=1 */
    setReg8(FPROT, 255);                 /* Set protection of flash block #2 */ 
    /* FCNFG: CBEIE=0,CCIE=0,KEYACC=0,??=0,??=0,??=0,BKSEL1=1,BKSEL0=1 */
    setReg8(FCNFG, 3);                   /* Select the flash block #3 */ 
    /* FSTAT: CBEIF=0,CCIF=0,PVIOL=1,ACCERR=1,??=0,BLANK=0,??=0,??=0 */
    setReg8(FSTAT, 48);                  /* Clear error flags PVIOL and ACCERR */ 
    /* FPROT: FPOPEN=1,NV6=1,FPHDIS=1,FPHS1=1,FPHS0=1,FPLDIS=1,FPLS1=1,FPLS0=1 */
    setReg8(FPROT, 255);                 /* Set protection of flash block #3 */ 
    /* FCNFG: CBEIE=0,CCIE=0,KEYACC=0,??=0,??=0,??=0,BKSEL1=0,BKSEL0=0 */
    setReg8(FCNFG, 0);                   /* Select the flash block #0 */ 
    /* FSTAT: CBEIF=0,CCIF=0,PVIOL=1,ACCERR=1,??=0,BLANK=0,??=0,??=0 */
    setReg8(FSTAT, 48);                  /* Clear error flags PVIOL and ACCERR */ 
    /* FPROT: FPOPEN=1,NV6=1,FPHDIS=1,FPHS1=1,FPHS0=1,FPLDIS=1,FPLS1=1,FPLS0=1 */
    setReg8(FPROT, 255);                 /* Set protection of flash block #0 */ 
#else
    if (bus_frequency > 12800000) {
        MCF_CFM_CFMCLKD = MCF_CFM_CFMCLKD_PRDIV8|MCF_CFM_CFMCLKD_DIV((bus_frequency/8-1)/200000);
    } else {
        MCF_CFM_CFMCLKD = MCF_CFM_CFMCLKD_DIV((bus_frequency-1)/200000);
    }
#endif

#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    MCF_CFM_CFMPROT = 0;
    MCF_CFM_CFMSACC = 0;
    MCF_CFM_CFMDACC = 0;
    MCF_CFM_CFMMCR = 0;
#endif
#endif
}

