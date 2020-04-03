// *** clone.c ********************************************************
// this file clones the contents of flash to the qspi master port,
// which can be connected to the ezport on a slave cpu for initial
// code load.

#include "main.h"

#if MCF52221 || MCF52259 || MCF5211
#define SLAVE_FSYS    8000000  // frequency of target board
#elif MCF52233
#define SLAVE_FSYS    5000000  // frequency of target board
#endif

#define CLONE_PAGE_SIZE  256

#define WRITE_CONFIGURATION_REGISTER  0x01
#define PAGE_PROGRAM  0x02
#define READ_DATA  0x03
#define READ_STATUS_REGISTER  0x05
#define WRITE_ENABLE  0x06
#define BULK_ERASE  0xc7

#define WRITE_ERROR  0x40
#define WRITE_ENABLE_STATUS  0x02
#define WRITE_IN_PROGRESS  0x01

static
int
clone_status(void)
{
    byte buffer[2];

    // read the status register
    buffer[0] = READ_STATUS_REGISTER;
    qspi_transfer(true, buffer, 1+1);

    return buffer[1];
}

static
bool
clone_write(byte *buffer, int length)
{
    byte status;
    byte command;

    status = clone_status();
    if (status & (WRITE_ERROR|WRITE_IN_PROGRESS)) {
        return false;
    }

    // enable the write
    command = WRITE_ENABLE;
    qspi_transfer(true, &command, 1);

    status = clone_status();
    if (! (status & WRITE_ENABLE_STATUS)) {
        return false;
    }

    // do the write
    qspi_transfer(true, buffer, length);

    // wait for the write to complete
    do {
        status = clone_status();
        if (status & WRITE_ERROR) {
            return false;
        }
    } while (status & WRITE_IN_PROGRESS);

    return true;
}

// this function resets the ezport device.
void
clone_reset(void)
{
    // pulse slave rsti* low
    pin_set(pin_assignments[pin_assignment_clone_rst], pin_type_digital_output, 0, 0);
    delay(1);
    pin_set(pin_assignments[pin_assignment_clone_rst], pin_type_digital_output, 0, 1);
    delay(1);
}

#ifndef MCF_CFM_CFMCLKD_DIV
#define MCF_CFM_CFMCLKD_DIV(x)               (((x)&0x3F)<<0)
#endif
#ifndef MCF_CFM_CFMCLKD_PRDIV8
#define MCF_CFM_CFMCLKD_PRDIV8               (0x40)
#endif

// this function initializes the ezport device and erases its flash.
bool
clone_init(uint32 slave_fsys)
{
    byte status;
    byte buffer[4];

    qspi_initialize();

    // enter ezport mode
    pin_set(pin_assignments[pin_assignment_qspi_cs], pin_type_digital_output, 0, 0);
    clone_reset();
    pin_set(pin_assignments[pin_assignment_qspi_cs], pin_type_digital_output, 0, 1);

    status = clone_status();
    if (status & (WRITE_ERROR|WRITE_IN_PROGRESS)) {
        printf("initialization failed\n");
        return false;
    }

    // write configuration register
    buffer[0] = WRITE_CONFIGURATION_REGISTER;
    if ((slave_fsys-1)/2/200000 >= 64) {
        buffer[1] = MCF_CFM_CFMCLKD_PRDIV8|MCF_CFM_CFMCLKD_DIV((slave_fsys-1)/2/8/200000);
    } else {
        buffer[1] = MCF_CFM_CFMCLKD_DIV((slave_fsys-1)/2/200000);
    }
    if (! clone_write(buffer, 2+0)) {
        printf("write configuration register failed\n");
        return false;
    }

    // bulk erase
    buffer[0] = BULK_ERASE;
    if (! clone_write(buffer, 1+0)) {
        printf("bulk erase failed\n");
        return false;
    }
    
    return true;
}

// this function programs a range of the ezport device.
bool
clone_program(uint32 addr, uint32 *data, uint32 length)
{
    uint32 i;
    
    ASSERT(4+length <= sizeof(big_buffer));
    
    // program the data to the target device
    big_buffer[0] = PAGE_PROGRAM;
    big_buffer[1] = addr/0x10000;
    big_buffer[2] = addr/0x100;
    big_buffer[3] = addr;
    if (data != (void *)-1) {
        memcpy(big_buffer+4, data, length);
    } else {
        memset(big_buffer+4, -1, length);
    }
    if (! clone_write(big_buffer, 4+length)) {
        printf("\npage program failed\n");
        return false;
    }


    // read the data back from the target device
    big_buffer[0] = READ_DATA;
    big_buffer[1] = addr/0x10000;
    big_buffer[2] = addr/0x100;
    big_buffer[3] = addr;
    memset(big_buffer+4, 0x5a, length);
    qspi_transfer(true, big_buffer, 4+length);

    // verify the data
    if (data != (void *)-1) {
        // reference data came from our flash
        if (memcmp(big_buffer+4, data, length)) {
            printf("\nverification failed in block at offset 0x%x\n", addr);
            return false;
        }
    } else {
        // reference data was erased
        for (i = 0; i < length; i++) {
            if (*(char *)(big_buffer+4+i) != (char)-1) {
                printf("\nverification failed at offset 0x%x\n", addr+i);
                return false;
            }
        }
    }
    
    return true;
}

void
clone(bool and_run)
{
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    uint32 addr;
    uint32 *data;

#if ! FLASHER
    // leave zigflea in reset while we use the qspi chip select for clone
    pin_set(pin_assignments[pin_assignment_zigflea_rst], pin_type_digital_output, 0, 0);
#endif
    
    if (clone_init(SLAVE_FSYS)) {
        // for all bytes to clone...
        for (addr = 0; addr < FLASH_BYTES; addr += CLONE_PAGE_SIZE) {
            if (SECURE && addr >= FLASH_BYTES/2) {
                // reference data is secure; just erase it
                data = (void *)-1;
            } else {
                // get the reference data from our flash
                data = (uint32 *)addr;
            }
            
            if (! clone_program(addr, data, CLONE_PAGE_SIZE)) {
                break;
            }

#if ! FLASHER
            printf(".");
#else
            if (addr/CLONE_PAGE_SIZE % 2) {
                printf(".");
            }
            if (addr/CLONE_PAGE_SIZE % 160 == 159) {
                printf("\n");
            }
#endif
        }

        if (and_run) {
            // allow the target to run!
            clone_reset();
        }

        printf("\nclone done!\n");
    }
    
#if ! FLASHER
    zb_initialize();
#endif
#endif
}

