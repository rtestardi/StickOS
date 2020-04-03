// *** clone.h ********************************************************

extern void clone_reset(void);
extern bool clone_init(uint32 slave_fsys);
extern bool clone_program(uint32 addr, uint32 *data, uint32 length);

extern void clone(bool and_run);

