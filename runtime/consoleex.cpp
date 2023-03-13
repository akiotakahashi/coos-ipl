#include "runtime.h"
#include "console.h"
#include "utility.h"


static TEXT (*vram)[80] = reinterpret_cast<TEXT(*)[80]>(0xB8000);

extern void PutCharacter(char ch, byte at, int x, int y) {
	vram[y][x].ch = ch;
	vram[y][x].at = at;
}

void Console::Initialize() {
	vram = reinterpret_cast<TEXT(*)[80]>(0xB8000);
}

void Console::Put(int x, int y, unsigned char ch, unsigned char at) {
	if(y<0 || y>=this->getBufferHeight()) panic("Illegal y-pos");
	if(x<0 || x>=this->getWidth()) panic("Illegal x-pos");
	getLineBuffer(y)[x] = TEXT(ch,at);
	if(IsVisible(y)) {
		TEXT& p = vram[y-baseline+y0][x+x0];
		p.ch = ch;
		p.at = at;
	}
}

static void* mymemcpy(void* _dst, const void* _src, unsigned int count) {
	__asm {
		mov	ecx, count
		mov	esi, _src
		mov	edi, _dst
		rep movsb
	}
	return _dst;
}

void Console::Refresh() {
	for(int y=baseline; y<baseline+getWindowHeight(); ++y) {
		mymemcpy(&vram[y-baseline+y0][x0], getLineBuffer(y), sizeof(TEXT)*getWidth());
	}
}

void Console::RollUp() {
	TEXT* buf = scrbuf[0];
	memmove(&scrbuf[0], &scrbuf[1], sizeof(scrbuf[0])*(linecount-1));
	memclr(buf, sizeof(TEXT)*getWidth());
	scrbuf[linecount-1] = buf;
	Refresh();
}
