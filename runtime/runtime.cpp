#include "runtime.h"
#include "memory.h"
#include "console.h"
#include "interrupt.h"
#include "pic.h"

extern volatile int intref = 1;	// Interruption is disabled initially.


extern const char* getKernelName() {
	return "CooS";
}

extern const char* getKernelVersion() {
	return "0.4";
}


void SegmentData::dump(Console& console) const {
	console << " cs:" << "----------" << "  ds:" << ds << "  ss:" << ss << endl;
	console << " es:" << es << "  fs:" << fs << "  gs:" << gs << endl;
}

void RegisterData::dump(Console& console) const {
	console << "eax:" << eax << " ebx:" << ebx << " ecx:" << ecx << " edx:" << edx << endl;
	console << "edi:" << edi << " esi:" << esi << " ebp:" << ebp << " esp:" << esp << endl;
}

static byte* frame;
static int pixelsize;
static int scanline;
static int xresolution;
static int yresolution;

#if 0

static void TimerHandler() {
	static int count = 0;
	if(frame) {
		if(0==(count&0x3)) {
			int i0 = count >> 2;
			for(int n=0; n<5; ++n) {
				int i = (i0-n+16)%16;
				int x = xresolution-3+(i-16)*16;
				for(int dy=0; dy<3; ++dy) {
					memset(frame+(3+dy)*scanline+x*pixelsize, 0xFF>>n, pixelsize*15);
				}
			}
		}
	} else {
		static char rotch[] = {'|','/','-','\\'};
		PutCharacter(rotch[(count>>0)&0x3], 0x0F, 79, 0);
		PutCharacter(rotch[(count>>3)&0x3], 0x0F, 78, 0);
		PutCharacter(rotch[(count>>6)&0x3], 0x0F, 77, 0);
	}
	++count;
}

typedef std::deque<KeyCode> KeyCodeList;
KeyCodeList* typedkeys = NULL;

extern void KeyboardHandler(KeyCode ch, KeyStatus ks) {
	Console& console = getConsole();
	if(ks==KS_DOWN) {
		Console& console = getConsole();
		switch(ch) {
		case KEY_PAGEUP:
			console.setWindowBaseline(console.getWindowBaseline()-console.getWindowHeight()/2-1);
			break;
		case KEY_PAGEDOWN:
			console.setWindowBaseline(console.getWindowBaseline()+console.getWindowHeight()/2+1);
			break;
		default:
			typedkeys->push_back(ch);
			break;
		}
	}
}


#include "kernelimpl.h"

struct Kernel : KernelImpl {
	virtual bool __stdcall NotAsSystem() {
		return false;
	}
	virtual void* __stdcall ReadLine() {
		return NULL;
	}
	virtual void __stdcall Write(wchar_t ch) {
		getConsole().Write((char)ch);
	}
};

static bool LoadAssemblyIntoManagedSpace(const std::wstring& assemname) {
	if(0==Commands::Execute(L"read 1 "+assemname+L".dll")) {
		Commands::IntroduceAssemblyIntoManaged(assemname, 1);
		return true;
	}
	getConsole() << "FAILED" << endl;
	return false;
}

#endif

static __declspec(naked) void __stdcall Div0ExceptionHandler() {
	InterruptHandler_Prologue;
	panic("DIVIDE BY ZERO");
	InterruptHandler_Epilogue;
}

static __declspec(naked) void __stdcall StackExceptionHandler() {
	InterruptHandler_Prologue;
	panic("STACK EXCEPTION");
	InterruptHandler_Epilogue;
}

static __declspec(naked) void __stdcall EmptyHandler() {
	InterruptHandler_Prologue;
	PIC::NotifyEndOfInterrupt(0x67);
	InterruptHandler_Epilogue;
}

extern uint64 AllocationCount;
extern uint64 TotalAllocationTime;

#if 0

static void* Read(const wchar_t* filename, int* sz) {
	Commands::Execute(std::wstring(L"read 0 ")+filename);
	getConsole() << "READ " << Commands::GetSlotSize(0) << " BYTES" << endl;
	*sz = Commands::GetSlotSize(0);
	return Commands::GetSlotData(0);
}

#endif

static void Free(void* p) {
	// NOP
}

extern mspace g_mspace = NULL;
extern "C" void mspace_malloc_stats(mspace msp);
extern "C" mspace create_mspace_with_base(void* base, size_t capacity, int locked);

struct ArgbHeader {
	union{
		// ピクセルの並び方を強く主張
		byte	abyIdentifier[4]; // "BGRA"
		uint32	dwIdentifier; // 0x41524742
	};
	uint32 PixelFormat; // 0x08080808(32bitColor) or 0x10101010(64bitColor)
	uint32 Width;
	uint32 Height;
};

#pragma warning(disable: 4005)
#include "../makeknl/kernel.exe.h"
#pragma warning(default: 4005)

extern __declspec(naked) int __stdcall DispatchMethod() {
	__asm {
		shr eax, 2;
		add eax, SECTION_ADDR_method_offsets;
		mov eax, [eax];
		add eax, SECTION_ADDR_methods;
		jmp eax;
	}
}

extern void kernelmain() {

	frame = *(byte**)(0x800+0x28);
	scanline = *(ushort*)(0x800+0x10);
	xresolution = *(ushort*)(0x800+0x12);
	yresolution = *(ushort*)(0x800+0x14);
	pixelsize = (*(byte*)(0x800+0x19)+7)/8;
	if(frame==NULL) {
		byte (*vram)[160] = reinterpret_cast<byte(*)[160]>(0xB8000);
		vram[0][4] = '<';
		vram[0][5] = 0xF;
		//memclr(vram, 2*25*80);
	} else {
		memclr(frame, yresolution*scanline);
	}

	uint32 cr0old, cr0new;
	__asm {
		finit;
	}
	__asm {
		mov eax, cr0;
		mov cr0old, eax;
		// FPU
		or  eax,  0x02;			// MP
		and eax, ~0x04;			// EM
		and eax, ~0x20;			// NE
		// Cache
		and eax, ~0x60000000;	// CD & NW
		//
		mov cr0, eax;
		mov cr0new, eax;
	}

	uint memkbsz = *(uint*)0x06FC;
	uint avlmem = memkbsz;
	avlmem *= 1024;

	const uint knlmemsz = 1024*1024*4;
	g_mspace = create_mspace_with_base((void*)knlmemsz, avlmem-knlmemsz, 0);
	/*
	MemoryManager mm((void*)(avlmem-knlmemsz), knlmemsz);
	setMemoryManager(mm);
	*/

	Console::Initialize();
	Console caption(0, 80, 0, 2, 1);
	caption.Clear();

	caption << TextColor(15,0);
	caption << TextColor(10) << getKernelName();
	caption << TextColor( 7) << " version ";
	caption << TextColor(12) << getKernelVersion();
	caption << TextColor( 7) << " built on " ;
	caption << TextColor( 9) << __DATE__ " " __TIME__;
	caption << TextColor( 7) << "  Mem: " << (int)memkbsz << "KB";

	Console workarea(0, 80,  2, 24, 10);
	Console infoarea(0, 80, 24, 25, 1);
	infoarea << TextColor(15,9);
	setConsole(workarea);
	infoarea << clrs << "Press CTRL+ALT+DEL to reset";

	//----------

	workarea << "Physical Memory " << (int)(memkbsz/1024) << " MB (" << (int)memkbsz << " KB)" << endl;

	//----------

	workarea << "Initialize Interrupt Descriptor Table" << endl;
	Interrupt::Initialize();

	//----------

	workarea << "Initialize Programmable Interrupt Controllers" << endl;
	PIC::Initialize();

	//----------

#if 0

	workarea << "Initialize Threading" << endl;
	Threading::Initialize();

	//----------

	workarea << "Enable Interruption" << endl;
	EnableInterrupt();

	//----------

	KeyCodeList _typedkeys;
	typedkeys = &_typedkeys;
	Keyboard::setKeyboardHandler(KeyboardHandler);

	workarea << "Initialize DMA Controllers" << endl;
	DMA::Initialize();
	
	workarea << "Initialize Keyboard Controller" << endl;
	Keyboard::Initialize();
	
	workarea << "Initialize Floppy Disk Controllers" << endl;
	FDD::Initialize();

	workarea << "Initialize Programmable Interval Timer" << endl;
	Timer::Initialize(8*8);

#endif

	//----------

	// QEMU occurs Int#0 and IRQ#7 unexpected exception. 
	Interrupt::RegisterInterruptGate(0x00, Div0ExceptionHandler);
	Interrupt::RegisterInterruptGate(0x0C, StackExceptionHandler);
	PIC::RegisterInterruptHandler(7, EmptyHandler);

	//----------

	byte* ptr = (byte*)SECTION_ADDR_plt;
	*(ptr++) = 0xe9;
	*(uint*)ptr = (uint)DispatchMethod-SECTION_ADDR_plt-5;

	//----------

	for(int entry = SECTION_ADDR_plt, i=0; entry < SECTION_ADDR_plt_end; entry += 0x10, ++i) {
		*(byte*)entry = 0xE9;
		*(uint*)(entry+1) = (SECTION_ADDR_methods+((uint*)SECTION_ADDR_method_offsets)[i-1])-(entry+5);
	}

	//----------

	for(int entry = SECTION_ADDR_got; entry < SECTION_ADDR__initialized; entry += 4) {
		*(unsigned int*)entry = entry+16*1024*1024;
	}

	//----------

	__asm { xchg bx,bx }
	((void (__fastcall *)(int))SECTION_ADDR___Lm_2)(0);
	panic("returned from kernel");

}

extern int main() {
	panic("main was called.");
}

extern void Delay(int nanoseconds) {
	while(nanoseconds>0) {
		__asm {
			nop;
			nop;
			nop;
			nop;
			nop;
			nop;
			//nop;
			//nop;
			//nop;
			//nop;
		}
		nanoseconds -= 3;
	}
}
