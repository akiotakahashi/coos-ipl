#include <memory.h>


/*
extern "C" extern void* memcpy(void* _dst, const void* _src, unsigned int count) {
	__asm {
		mov	ecx, count
		mov	esi, _src
		mov	edi, _dst
		rep movsb
	}
	return _dst;
}
*/

extern "C" extern void* memmove(void* _dst, const void* _src, unsigned int count) {
	unsigned char* dst = (unsigned char*)_dst;
	const unsigned char* src = (const unsigned char*)_src;
	if(src<dst) {
		while(count>0) {
			--count;
			dst[count] = src[count];
		}
	} else {
		unsigned int i = 0;
		while(i<count) {
			dst[i] = src[i];
			++i;
		}
	}
	return _dst;
}

#pragma function(memcmp)

extern "C" extern int memcmp(const void* p1, const void* p2, size_t size) {
	while(size--) {
		unsigned char q1 = *(const unsigned char*)p1;
		unsigned char q2 = *(const unsigned char*)p2;
		p1 = (const unsigned char*)p1+1;
		p2 = (const unsigned char*)p2+1;
		if(q1<q2) return -1;
		if(q1>q2) return +1;
	}
	return 0;
}
