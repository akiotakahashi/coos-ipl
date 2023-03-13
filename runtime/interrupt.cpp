#include "interrupt.h"
#include "runtime.h"
#include "stdlib.h"


#define DECLARE_DUMMY_INTERRUPT_HANDLER(n) \
	static void __stdcall _os_dummy_##n() { \
	panic("DUMMY INTERRUPT HANDLER #" #n " WAS CALLED"); }

#define DECLARE_DUMMY_INTERRUPT_HANDLER_16(m) \
	DECLARE_DUMMY_INTERRUPT_HANDLER(m ## 0); \
	DECLARE_DUMMY_INTERRUPT_HANDLER(m ## 1); \
	DECLARE_DUMMY_INTERRUPT_HANDLER(m ## 2); \
	DECLARE_DUMMY_INTERRUPT_HANDLER(m ## 3); \
	DECLARE_DUMMY_INTERRUPT_HANDLER(m ## 4); \
	DECLARE_DUMMY_INTERRUPT_HANDLER(m ## 5); \
	DECLARE_DUMMY_INTERRUPT_HANDLER(m ## 6); \
	DECLARE_DUMMY_INTERRUPT_HANDLER(m ## 7); \
	DECLARE_DUMMY_INTERRUPT_HANDLER(m ## 8); \
	DECLARE_DUMMY_INTERRUPT_HANDLER(m ## 9); \
	DECLARE_DUMMY_INTERRUPT_HANDLER(m ## A); \
	DECLARE_DUMMY_INTERRUPT_HANDLER(m ## B); \
	DECLARE_DUMMY_INTERRUPT_HANDLER(m ## C); \
	DECLARE_DUMMY_INTERRUPT_HANDLER(m ## D); \
	DECLARE_DUMMY_INTERRUPT_HANDLER(m ## E); \
	DECLARE_DUMMY_INTERRUPT_HANDLER(m ## F);

DECLARE_DUMMY_INTERRUPT_HANDLER_16(0);
DECLARE_DUMMY_INTERRUPT_HANDLER_16(1);
DECLARE_DUMMY_INTERRUPT_HANDLER_16(2);
DECLARE_DUMMY_INTERRUPT_HANDLER_16(3);
DECLARE_DUMMY_INTERRUPT_HANDLER_16(4);
DECLARE_DUMMY_INTERRUPT_HANDLER_16(5);
DECLARE_DUMMY_INTERRUPT_HANDLER_16(6);
DECLARE_DUMMY_INTERRUPT_HANDLER_16(7);
DECLARE_DUMMY_INTERRUPT_HANDLER_16(8);
DECLARE_DUMMY_INTERRUPT_HANDLER_16(9);
DECLARE_DUMMY_INTERRUPT_HANDLER_16(A);
DECLARE_DUMMY_INTERRUPT_HANDLER_16(B);
DECLARE_DUMMY_INTERRUPT_HANDLER_16(C);
DECLARE_DUMMY_INTERRUPT_HANDLER_16(D);
DECLARE_DUMMY_INTERRUPT_HANDLER_16(E);
DECLARE_DUMMY_INTERRUPT_HANDLER_16(F);

#define REGISTER_DUMMY_INTERRUPT_HANDLER(n) \
	RegisterInterruptGate(0x ## n, _os_dummy_##n);

#define REGISTER_DUMMY_INTERRUPT_HANDLER_16(m) \
	REGISTER_DUMMY_INTERRUPT_HANDLER(m ## 0); \
	REGISTER_DUMMY_INTERRUPT_HANDLER(m ## 1); \
	REGISTER_DUMMY_INTERRUPT_HANDLER(m ## 2); \
	REGISTER_DUMMY_INTERRUPT_HANDLER(m ## 3); \
	REGISTER_DUMMY_INTERRUPT_HANDLER(m ## 4); \
	REGISTER_DUMMY_INTERRUPT_HANDLER(m ## 5); \
	REGISTER_DUMMY_INTERRUPT_HANDLER(m ## 6); \
	REGISTER_DUMMY_INTERRUPT_HANDLER(m ## 7); \
	REGISTER_DUMMY_INTERRUPT_HANDLER(m ## 8); \
	REGISTER_DUMMY_INTERRUPT_HANDLER(m ## 9); \
	REGISTER_DUMMY_INTERRUPT_HANDLER(m ## A); \
	REGISTER_DUMMY_INTERRUPT_HANDLER(m ## B); \
	REGISTER_DUMMY_INTERRUPT_HANDLER(m ## C); \
	REGISTER_DUMMY_INTERRUPT_HANDLER(m ## D); \
	REGISTER_DUMMY_INTERRUPT_HANDLER(m ## E); \
	REGISTER_DUMMY_INTERRUPT_HANDLER(m ## F);

namespace Interrupt {

#include "enpack.h"
#include "enalign.h"

	struct InterruptDescriptorTable {
		uint16 limit;
		uint32 base;
	};

	struct InterruptDescriptor {
		uint16 handler_l;
		uint16 segment;
		uint16 flags		: 13;
		uint16 dpl			: 2;
		uint16 p			: 1;
		uint16 handler_h;
	};

#include "unpack.h"
#include "unalign.h"

	static InterruptDescriptor idt[256];

	extern void Initialize() {
		InterruptDescriptorTable buf;
		for(int i=0; i<sizeof(idt)/sizeof(idt[0]); ++i) {
			idt[i].handler_l = NULL;
			idt[i].handler_h = NULL;
			idt[i].segment = 0x08;
			idt[i].flags = 0xE00;
			idt[i].dpl = 0;
			idt[i].p = 1;
		}
		buf.base = (uint32)&idt[0];
		buf.limit = sizeof(idt)-1;
		__asm { lidt buf }
		REGISTER_DUMMY_INTERRUPT_HANDLER_16(0);
		REGISTER_DUMMY_INTERRUPT_HANDLER_16(1);
		REGISTER_DUMMY_INTERRUPT_HANDLER_16(2);
		REGISTER_DUMMY_INTERRUPT_HANDLER_16(3);
		REGISTER_DUMMY_INTERRUPT_HANDLER_16(4);
		REGISTER_DUMMY_INTERRUPT_HANDLER_16(5);
		REGISTER_DUMMY_INTERRUPT_HANDLER_16(6);
		REGISTER_DUMMY_INTERRUPT_HANDLER_16(7);
		REGISTER_DUMMY_INTERRUPT_HANDLER_16(8);
		REGISTER_DUMMY_INTERRUPT_HANDLER_16(9);
		REGISTER_DUMMY_INTERRUPT_HANDLER_16(A);
		REGISTER_DUMMY_INTERRUPT_HANDLER_16(B);
		REGISTER_DUMMY_INTERRUPT_HANDLER_16(C);
		REGISTER_DUMMY_INTERRUPT_HANDLER_16(D);
		REGISTER_DUMMY_INTERRUPT_HANDLER_16(E);
		REGISTER_DUMMY_INTERRUPT_HANDLER_16(F);
	}

	static void SetGate(uint16 flags, byte intno, INTERRUPT_HANDLER* handler) {
		InterruptDescriptor& id = idt[intno];
		DisableInterrupt();
		if(handler==NULL) {
			memclr(&id, sizeof id);
		} else {
			unsigned short rCS;
			__asm { mov rCS, cs }
			id.handler_l =  0x0000FFFF & (uint)handler;
			id.handler_h = (0xFFFF0000 & (uint)handler) >> 16;
			id.segment = rCS;
			id.flags = flags;
			id.dpl = 0;
			id.p = 1;
		}
		InterruptDescriptorTable buf;
		buf.base = (uint32)&idt[0];
		buf.limit = sizeof(idt)-1;
		__asm lidt buf;
		EnableInterrupt();
	}

	extern void RegisterInterruptGate(byte intno, INTERRUPT_HANDLER* handler) {
		SetGate(0xE00, intno, handler);
	}

	extern void RegisterTrapGate(byte intno, INTERRUPT_HANDLER* handler) {
		SetGate(0xF00, intno, handler);
	}

}
