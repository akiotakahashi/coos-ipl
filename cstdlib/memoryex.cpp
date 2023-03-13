#include "cstdlib.h"


extern "C" extern void memclr(void* addr, size_t size) {
#if 0
	__asm {
		mov	eax, 0;
		mov	edi, p;
		mov	ecx, sz;
		shr	ecx, 2;
		rep stosd;
		mov	ecx, sz;
		and	ecx, 0x3;
		rep stosb;
	}
#else
	unsigned char* p = (unsigned char*)addr;
	for(size_t count=0; count<size; ++count) {
		*(p++) = 0;
	}
#endif
}

#pragma function(memset)

extern "C" extern void* memset(void* addr, int val, size_t size) {
#if 0
	__asm {
		mov	eax, val;
		mov	edi, p;
		mov	ecx, sz;
		shr	ecx, 2;
		rep stosd;
		mov	ecx, sz;
		and	ecx, 0x3;
		rep stosb;
	}
#else
	unsigned char* p = (unsigned char*)addr;
	for(size_t count=0; count<size; ++count) {
		*(p++) = val;
	}
#endif
	return addr;
}
