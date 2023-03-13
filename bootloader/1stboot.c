#define CODE_SEGMENT	0x07C6
#define DATA_SEGMENT	0x07DC
#include "bootldr.h"


int boot() {
	prints("CooS 1st bootloader...");
	read(0x0100, 0, 2, 8);
	prints("OK\r\n");
}
