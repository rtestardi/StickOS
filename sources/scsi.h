#define FLAGS_DATA_OUT  ((char)0x00)
#define FLAGS_DATA_IN  ((char)0x80)

struct cbw {  // command block wrapper
    int signature;  //0x43425355
    int tag;
    int datatransferlength;
    byte flags;
    byte lun;
    byte cdblength;
    byte cdb[16];
};

#define CBW_LENGTH  31  // ignore struct padding

struct csw {  // command status wrapper
    int signature;  //0x43425355
    int tag;
    int dataresidue;
    byte status;
};

#define CSW_LENGTH  13  // ignore struct padding

extern byte scsi_lun;

// perform a usb host/device scsi bulk transfer
int
scsi_bulk_transfer(int in, byte *cdb, int cdb_length, byte *buffer, int length);

void
scsi_run(void);

