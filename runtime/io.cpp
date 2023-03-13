#include "io.h"

#pragma unmanaged


extern void outp8(unsigned short port, byte data) {
	__asm {
		mov		dx, port;
		mov		al, data;
		out		dx, al;
	}
}

extern void outp16(unsigned short port, uint16 data) {
	__asm {
		mov		dx, port;
		mov		ax, data;
		out		dx, ax;
	}
}

extern void outp32(unsigned short port, uint32 data) {
	__asm {
		mov		dx, port;
		mov		eax, data;
		out		dx, eax;
	}
}

extern byte inp8(unsigned short port) {
	byte data;
	__asm {
		mov		dx, port;
		in		al, dx;
		mov		data, al;
	}
	return data;
}

extern ushort inp16(unsigned short port) {
	ushort data;
	__asm {
		mov		dx, port;
		in		ax, dx;
		mov		data, ax;
	}
	return data;
}

extern ulong inp32(unsigned short port) {
	ulong data;
	__asm {
		mov		dx, port;
		in		eax, dx;
		mov		data, eax;
	}
	return data;
}

extern void inp16(unsigned short port, void* p, int count, unsigned short watchport, unsigned char mask, unsigned char value) {
	while(count>0) {
		if(value==(mask&inp8(watchport))) {
			ushort data = inp16(port);
			*(ushort*)p = data;
			p = (ushort*)p+1;
			--count;
		} else {
			inp8(0);
		}
	}
}
