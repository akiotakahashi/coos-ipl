#include "cstdlib.h"


extern "C" extern void abort() {
	__asm {
		cli;
begin:
		hlt;
		jmp	begin;
	}
}
