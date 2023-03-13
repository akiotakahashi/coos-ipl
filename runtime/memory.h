#pragma once

#include "stdtype.h"
#include "stdlib.h"


#if 0

struct Unit;

struct UnitList {
	uint magic;
	Unit* head;
	Unit* tail;
	UnitList(uint _magic) : magic(_magic) {
		head = NULL;
		tail = NULL;
	}
	void insert(Unit* unit);
	void remove(Unit* unit);
	void replace(Unit* from, Unit* to);
};

class MemoryRegion {
	typedef unsigned char byte;
	byte* base;
	uint sz;
	bool readonly;
public:
	MemoryRegion(void* p, uint _sz) {
		readonly = false;
		base = (byte*)p;
		sz = _sz;
	}
	MemoryRegion(const void* p, uint _sz) {
		readonly = true;
		base = (byte*)p;
		sz = _sz;
	}
	MemoryRegion(const MemoryRegion& memrgn) {
		*this = memrgn;
	}
public:
	uint size() const { return sz; }
public:
	const byte* operator +(int offset) const { return base+offset; }
	const byte& operator [](int offset) const { return base[offset]; }
	byte* operator +(int offset) { if(readonly)panic("READONLY MEM"); return base+offset; }
	byte& operator [](int offset) { if(readonly)panic("READONLY MEM"); return base[offset]; }
};

class MemoryManager {
	byte* base;
	int size;
	UnitList used;
	UnitList free;
public:
	MemoryManager(void* b, uint s);
	void Initialize(void* b, uint s);
public:
	void Dump();
	void* Reallocate(void* p, uint size);
	void* Allocate(size_t size);
	void Release(void* p);
};


extern MemoryManager& getMemoryManager();
extern void setMemoryManager(MemoryManager& mm);

// operator new/delete
static inline void* operator new(size_t size) { return getMemoryManager().Allocate(size); }
static inline void operator delete(void* p) { return getMemoryManager().Release(p); }
static inline void* operator new[](size_t size) { return getMemoryManager().Allocate(size); }
static inline void operator delete[](void* p) { return getMemoryManager().Release(p); }

#else

typedef void* mspace;
typedef unsigned int size_t;
extern mspace g_mspace;
extern "C" mspace mspace_malloc(mspace msp, size_t bytes);
extern "C" void mspace_free(mspace msp, void* mem);
static inline void* operator new(size_t size){ return mspace_malloc(g_mspace,size); }
static inline void* operator new[](size_t size){ return mspace_malloc(g_mspace,size); }
static inline void operator delete(void* p) { mspace_free(g_mspace,p); }
static inline void operator delete[](void* p) { mspace_free(g_mspace,p); }
static inline void* operator new(size_t, void* _Where) { return (_Where); }
static inline void* operator new[](size_t, void* _Where) { return (_Where); }
static inline void operator delete(void*, void*) { }
static inline void operator delete[](void*, void*) { }

#endif
