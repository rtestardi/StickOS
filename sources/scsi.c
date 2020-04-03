// *** scsi *****************************************************************

#include "main.h"
#if USBOTG
#if ! STICKOS || USB_HOST

byte scsi_lun;

// perform a usb host/device scsi bulk transfer
int
scsi_bulk_transfer(int in, byte *cdb, int cdb_length, byte *buffer, int length)
{
    int rv;
    int total;
    struct cbw cbw;
    struct csw csw;
    static int tag;

    memset(&cbw, 0, sizeof(cbw));
    cbw.signature = TF_LITTLE(0x43425355);
    cbw.tag = tag++;
    cbw.datatransferlength = TF_LITTLE(length);
    cbw.flags = in?FLAGS_DATA_IN:FLAGS_DATA_OUT;
    cbw.lun = scsi_lun;
    cbw.cdblength = (byte)cdb_length;
    if (scsi_attached == 2) {
        cbw.cdblength = 12;
    }
    memset(cbw.cdb, 0, sizeof(cbw.cdb));  // revisit -- remove?
    memcpy(cbw.cdb, cdb, cdb_length);

    rv = usb_bulk_transfer(0, (byte *)&cbw, CBW_LENGTH, 0);
    if (rv < 0) {
        return rv;
    }
    assert(rv == CBW_LENGTH);

    total = usb_bulk_transfer(in, buffer, length, 0);

    rv = usb_bulk_transfer(1, (byte *)&csw, CSW_LENGTH, 0);
    if (rv == -TOKEN_STALL) {
        // mst stall disaster
        rv = usb_bulk_transfer(1, (byte *)&csw, CSW_LENGTH, 0);
    }
    
    if (rv < 0) {
        return rv;
    }
    
    assert(rv == CSW_LENGTH);
    assert(csw.signature == TF_LITTLE(0x53425355));
    assert(csw.tag == cbw.tag);
    if (csw.status) {
        return -csw.status;  // revisit some devices lie!
    }
    //assert(TF_LITTLE(csw.dataresidue) == TF_LITTLE(cbw.datatransferlength) - total);
    assert(csw.status == 0);

    return total;
}
#endif
#endif

