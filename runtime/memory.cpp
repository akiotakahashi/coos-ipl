#define inline
#define static extern
#include "memory.h"
#undef static
#undef inline
#include "runtime.h"
#include "stdlib.h"
#include "console.h"


struct Mark {
	UnitList* owner;
	uint size;
	inline Unit* getUnit() { return (Unit*)((byte*)this-size); }
	inline Unit* getNextUnit() { return (Unit*)(this+1); }
};

struct Unit {
	Unit* prev;
	Unit* next;
	uint size;
	uint magic;
	Unit(uint s) {
		prev = NULL;
		next = NULL;
		magic = 0x1111ABCD;
		setSize(s);
	}
	inline void setSize(uint sz) {
		size = sz;
		getMark()->size = sz;
	}
	inline uint getWorkSize() { return size-sizeof(Unit); }
	inline byte* getBase() { return (byte*)(this+1); }
	inline Mark* getMark() { return (Mark*)((byte*)this+size); }
	inline Mark* getPrevMark() { return (Mark*)this-1; }
	inline Unit* getNextUnit() { return (Unit*)((byte*)this+size+sizeof(Mark)); }
};


void UnitList::insert(Unit* unit) {
	if(unit->magic!=0x1111ABCD) panic("ILLEGAL BLOCK WILL BE RESERVED");
	unit->magic = magic;
	if(head==NULL) {
		head = tail = unit;
		head->prev = NULL;
		tail->next = NULL;
	} else {
		unit->next = head;
		unit->prev = NULL;
		head->prev = unit;
		head = unit;
	}
	unit->getMark()->owner = this;
}

void UnitList::remove(Unit* unit) {
	if(unit->magic!=magic) panic("ILLEGAL BLOCK WILL BE RELEASED");
	unit->magic = 0x1111ABCD;
	if(unit==head) head = head->next;
	if(unit==tail) tail = tail->prev;
	if(unit->prev) unit->prev->next = unit->next;
	if(unit->next) unit->next->prev = unit->prev;
	unit->prev = NULL;
	unit->next = NULL;
}

void UnitList::replace(Unit* fm, Unit* to) {
	if(to->magic!=0x1111ABCD) panic("ILLEGAL BLOCK WILL BE INSERTED");
	if(fm->magic!=magic) panic("ILLEGAL BLOCK WILL BE DISPLACED");
	Unit* prev = fm->prev;
	Unit* next = fm->next;
	to->prev = prev;
	to->next = next;
	fm->prev = NULL;
	fm->next = NULL;
	if(prev) prev->next = to;
	if(next) next->prev = to;
	if(head==fm) head=to;
	if(tail==fm) tail=to;
	fm->magic = 0x1111ABCD;
	to->magic = magic;
	to->getMark()->owner = this;
}

MemoryManager::MemoryManager(void* b, uint s) : free(0x2222ABCD), used(0x3333ABCD) {
	base = (byte*)b;
	size = s;
	Unit* p = (Unit*)base;
	p = new(p)Unit(sizeof(Unit));
	used.insert(p);
	p = p->getNextUnit();
	p = new(p)Unit(size-(sizeof(Unit)+sizeof(Mark))*3+sizeof(Unit));
	free.insert(p);
	p = p->getNextUnit();
	p = new(p)Unit(sizeof(Unit));
	used.insert(p);
	if(&getMemoryManager()==NULL) {
		setMemoryManager(*this);
	}
}

void MemoryManager::Initialize(void* b, uint s) {
	new(this) MemoryManager(b,s);
}

void MemoryManager::Dump() {
	uint cntu = 0;
	uint sumu = 0;
	uint maxu = 0;
	void* hiau = NULL;
	getConsole() << "--- used memory blocks" << endl;
	for(Unit* p=used.head; p; p=p->next) {
		++cntu;
		sumu += p->size;
		if(maxu<p->size) maxu=p->size;
		if(p->getMark()->owner!=&used) {
			getConsole() << "{BROKEN:" << p << "," << p->getMark() << "," << p->getMark()->owner << "}" << endl;
		}
		if(hiau<p->getBase()+p->size) hiau=p->getBase()+p->size;
	}
	getConsole() << "Fragment Count  = " << (int)cntu << endl;
	getConsole() << "Total Size      = " << (int)sumu << endl;
	getConsole() << "Average Size    = " << (int)(sumu/cntu) << endl;
	getConsole() << "Maximum Size    = " << (int)maxu << endl;
	getConsole() << "Highest Address = " << hiau << endl;
	getConsole() << "--- free memory blocks" << endl;
	uint cntf = 0;
	uint sumf = 0;
	uint maxf = 0;
	void* hiaf = NULL;
	for(Unit* p=free.head; p; p=p->next) {
		++cntf;
		sumf += p->size;
		if(maxf<p->size) maxf=p->size;
		if(p->getMark()->owner!=&free) {
			getConsole() << "{BROKEN:" << p << "," << p->getMark() << "," << p->getMark()->owner << "}" << endl;
		}
		if(hiaf<p->getBase()+p->size) hiaf=p->getBase()+p->size;
	}
	getConsole() << "Fragment Count  = " << (int)cntf << endl;
	getConsole() << "Total   Size    = " << (int)sumf << endl;
	getConsole() << "Average Size    = " << (int)(sumf/cntf) << endl;
	getConsole() << "Maximum Size    = " << (int)maxf << endl;
	getConsole() << "Highest Address = " << hiaf << endl;
	getConsole() << "--- statistics" << endl;
	getConsole() << "Memory Space    = " << (uint)size << endl;
	getConsole() << "Scanned Size    = " << sumu+sumf+(cntu+cntf)*sizeof(Mark) << endl;

}

void* MemoryManager::Reallocate(void* _p, uint size) {
	getConsole() << "!";
	if(_p==NULL) {
		return Allocate(size);
	} else if(size==0) {
		Release(_p);
		return NULL;
	} else {
		Unit* p0 = (Unit*)_p-1;
		void* p1 = Allocate(size);
		memcpy(p1, p0->getBase(), p0->getWorkSize());
		Release(_p);
		return p1;
	}
}

extern uint64 AllocationCount = 0;
extern uint64 TotalAllocationTime = 0;

static uint64 rdtsc() {
	uint32 h,l;
	__asm {
		rdtsc;
		mov h,edx;
		mov l,eax;
	}
	return ((uint64)h<<32) | l;
}

void* MemoryManager::Allocate(uint size) {
	if(size==0) return NULL;
	uint64 t0 = rdtsc();
	size += sizeof(Unit);
	size = (size+7)&~0x7;
	Unit* p = free.head;
	while(p->size<size) {
		p = p->next;
		if(p==NULL) {
			getConsole().MakeNewLine();
			getConsole() << "********************";
			getConsole() << "********************";
			getConsole() << "********************";
			getConsole() << "*******************" << endl;
			getConsole() << "MEMORY ALLOCATION FAILED: " << size << " bytes" << endl;
			Dump();
			freeze;
		}
	}
	if(p->size-size<sizeof(Unit)+sizeof(Mark)+16) {
		// 残りのサイズが16byteより僅少になる場合
		free.remove(p);
		used.insert(p);
	} else {
		// 空き領域が十分に大きいので分割する場合
		Unit* f = new((byte*)p+size+sizeof(Mark)) Unit(p->size-size-sizeof(Mark));
		free.remove(p);
		free.insert(f);
		p->setSize(size);
		used.insert(p);
	}
	uint64 t1 = rdtsc();
	TotalAllocationTime += t1-t0;
	++AllocationCount;
	return p->getBase();
}

void MemoryManager::Release(void* _p) {
	if(_p==NULL) return;
//	getConsole() << "{release:" << _p << "}";
	uint64 t0 = rdtsc();
	Unit* p = (Unit*)_p - 1;

	used.remove(p);

	Mark* prevm = p->getPrevMark();
	Unit* nextu = p->getNextUnit();
	if(prevm->owner==&free && nextu->getMark()->owner==&free) {
		free.remove(p->getNextUnit());
		Unit* f = prevm->getUnit();
		f->setSize(f->size+p->size+nextu->size+sizeof(Mark)*2);
		f->getMark()->owner = &free;
	} else if(prevm->owner==&free) {
		Unit* q = prevm->getUnit();
		q->setSize(q->size+p->size+sizeof(Mark));
		q->getMark()->owner = &free;
	} else if(nextu->getMark()->owner==&free) {
		Unit* q = p->getNextUnit();
		p->setSize(p->size+q->size+sizeof(Mark));
		free.replace(q,p);
	} else {
		free.insert(p);
	}

	uint64 t1 = rdtsc();
	TotalAllocationTime += t1-t0;
	++AllocationCount;
}

static MemoryManager* pmm = NULL;

extern MemoryManager& getMemoryManager() {
	return *pmm;
}

extern void setMemoryManager(MemoryManager& mm) {
	pmm = &mm;
}
