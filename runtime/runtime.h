#pragma once

#include "stdtype.h"


#include "enalign.h"
#include "enpack.h"

class Console;

struct SegmentData {
	uint32 gs;
	uint32 fs;
	uint32 es;
	uint32 ds;
	uint32 ss;
	void dump(Console& console) const;
};

#define pushseg \
	__asm{push ss} \
	__asm{push ds} \
	__asm{push es} \
	__asm{push fs} \
	__asm{push gs}

#define popseg \
	__asm{pop gs} \
	__asm{pop fs} \
	__asm{pop es} \
	__asm{pop ds} \
	__asm{pop ss}

struct RegisterData {
	uint32 edi;
	uint32 esi;
	uint32 ebp;
	uint32 esp;
	uint32 ebx;
	uint32 edx;
	uint32 ecx;
	uint32 eax;
	void dump(Console& console) const;
};

#define pushreg	__asm{pushad}
#define popreg	__asm{popad}

struct InterruptData {
	uint32 eip;
	uint32 cs;
	uint32 eflags;
};

struct Context : SegmentData, RegisterData, InterruptData {
	void dump(Console& console) const {
		static_cast<const RegisterData*>(this)->dump(console);
		static_cast<const SegmentData*>(this)->dump(console);
	}
};

#include "unalign.h"
#include "unpack.h"


//************************************************************************************************

extern const char* getKernelName();
extern const char* getKernelVersion();

//************************************************************************************************

enum KeyStatus {
	KS_DOWN		= 1,
	KS_UP		= 2,
};

extern void _os_key_set_buffer(KeyCode kc, KeyStatus ks);

//************************************************************************************************

extern void Delay(int nanoseconds);

static inline void Delay9(int s) { Delay(s); }
static inline void Delay6(int s) { Delay(s<<10); }
static inline void Delay3(int s) { Delay(s<<20); }

static inline long long __declspec(naked) getTimestampCounter() { __asm { rdtsc } __asm { ret } }

//************************************************************************************************

extern volatile int intref;

inline void DisableInterrupt() {
	if(intref++==0) __asm { cli }
}

inline void EnableInterrupt() {
	if(--intref==0) __asm { sti }
}

//************************************************************************************************
