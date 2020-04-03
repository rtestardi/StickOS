// *** flasher.c ******************************************************
// this file implements the RAM project flasher, to clone flash memory
// to a target device via QSPI/EzPort.

#include "main.h"

void
main_run()
{
    clone(true);
    for (;;) {
        // NULL
    }
}

void
main_initialize(void)
{
}

