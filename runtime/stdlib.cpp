#include "stdlib.h"
#include "runtime.h"
#include "memory.h"
#include "console.h"


static void __stdcall dump_registers(bool unicode, SegmentData sd, RegisterData rd, const void* retip, const void* msg) {
	Console& console = getConsole();
	console.MakeNewLine();
	console << "KERNEL PANIC at " << retip << endl;
	rd.dump(console);
	//sd.dump(console);
	/*
	console << "--- Callstack ---";
	void** p = (void**)rd.ebp;
	for(int i=0; i<24 && p; ++i) {
		if(i%6==0) console << endl;
		console << *(p+1) << " ";
		p = (void**)*p;
	}
	console << endl;
	//*/
	//*
	console << "--- Stack Dump ---" << endl;
	uint32* sp_ = (uint32*)rd.esp+2;	// eip & msg
	for(int l=0; l<6; ++l) {
		console << sp_ << ":";
		for(int i=0; i<4; ++i) {
			console << " " << *sp_;
			++sp_;
		}
		console << endl;
	}
	//*/
	if(msg!=0) {
		if(unicode) {
			console << "*** " << (const wchar_t*)msg << " ***";
		} else {
			console << "*** " << (const char*)msg << " ***";
		}
	}
}

__declspec(naked) extern void dump(const char* msg) {
	pushreg;
	pushseg;
	__asm push 0;
	__asm call dump_registers;
}

static void abort() {
	for(;;) {
		__asm { cli }
		__asm { hlt }
	}
}

__declspec(naked) extern void panic(const char* msg) {
	//__asm cli;
	pushreg;
	pushseg;
	__asm push 0;
	__asm call dump_registers;
	abort();
}

__declspec(naked) extern void panic(const wchar_t* msg) {
	//__asm cli;
	pushreg;
	pushseg;
	__asm push 1;
	__asm call dump_registers;
	abort();
}

extern "C" extern void* malloc(size_t size) {
#if defined(getMemoryManager)
	return getMemoryManager().Allocate(size);
#else
	return new unsigned char[size];
#endif
}

extern "C" extern void free(void* p) {
#if defined(getMemeryManager)
	if(p) getMemoryManager().Release(p);
#else
	if(p) delete[] (unsigned char*)p;
#endif
}

extern "C" extern int puts(const char *string) {
	getConsole() << string << "\r\n";
	return EOF;
}

extern "C" extern int _purecall() {
	panic("PURE FUNCTION CALLED");
}

extern "C" int atexit(void(*func)()) {
	return 1;
}

//extern "C" FILE _iob[3] = {};

#if defined(FILE)

extern "C" int fprintf(FILE* file, const char* format, ...) {
	return 0;
}

#endif
