#define LAST_DMEM_ADDR (32*1024 - 1)
#define DATA_ADDR ((volatile int*)(LAST_DMEM_ADDR + 1))
#define BUSY_ADDR ((volatile char*)(LAST_DMEM_ADDR + 1 + 4))
