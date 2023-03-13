#include "runtime.h"
#include "console.h"
#include "utility.h"
#include "stdlib.h"


extern const char* endl = "\r\n";
extern const char* clrs = "\f";
extern const char* newline = "\v";


Console::Console(int X0, int X1, int Y0, int Y1, int bufscale) {
	x0 = X0;
	y0 = Y0;
	x1 = X1;
	y1 = Y1;
	Attribute = 0xF;
	linecount = (y1-y0)*bufscale;
	scrbuf = new TEXT*[linecount];
	for(int i=0; i<linecount; ++i) {
		scrbuf[i] = new TEXT[getWidth()];
	}
	Clear();
	if(&getConsole()==NULL) {
		setConsole(*this);
	}
}

Console::~Console() {
	for(int i=0; i<linecount; ++i) {
		delete [] scrbuf[i];
	}
	delete [] scrbuf;
}

void Console::Clear() {
	for(int i=0; i<linecount; ++i) {
		memclr(scrbuf[i], sizeof(TEXT)*getWidth());
	}
	baseline = 0;
	cx = 0;
	cy = 0;
	Refresh();
}

void Console::Back(int n) {
	while(n-->0) {
		if(cx>0) {
			--cx;
		} else if(cy>0) {
			--cy;
			cx = getWidth()-1;
		} else break;
	}
}

void Console::Write(char ch) {
	switch(ch) {
	case '\r':
		cx = 0;
		break;
	case '\n':
		if(cy<getBufferHeight()-1) {
			++cy;
			if(cy==baseline+getWindowHeight()) {
				++baseline;
			}
		} else {
			RollUp();
			if(baseline<getBufferHeight()-getWindowHeight()) {
				if(baseline>0) --baseline;
			}
		}
		Refresh();
		break;
	case '\v':
		MakeNewLine();
		break;
	case '\b':
		Back();
		break;
	case '\f':
		Clear();
		break;
	case '\t':
		cx = (cx+4)/4*4;
		break;
	default:
		if(cx==getWidth()) {
			Write('\r');
			Write('\n');
		}
		Put(cx++, cy, ch, Attribute);
		break;
	}
}

void Console::MakeNewLine() {
	if(cx!=0) Write(endl);
}

void Console::setWindowBaseline(int y) {
	if(y<0) {
		y = 0;
	} else {
		if(y>cy) y=cy;
		if(y>=getBufferHeight()-getWindowHeight()) {
			y = getBufferHeight()-getWindowHeight();
		}
	}
	baseline = y;
	Refresh();
}

Console& Console::operator <<(const TextColor& c) {
	Attribute = c.Attribute();
	return *this;
}

Console& Console::operator <<(const char* s) {
	char ch;
	while(ch=*(s++)) {
		Write(ch);
	}
	return *this;
}

Console& Console::operator <<(const wchar_t* s) {
	wchar_t ch;
	while(ch=*(s++)) {
		Write((char)ch);
	}
	return *this;
}

Console& Console::operator <<(byte n) {
	*this << "0x";
	Write(n2h((n&0x00F0)>>4));
	Write(n2h((n&0x000F)>>0));
	return *this;
}

Console& Console::operator <<(ushort n) {
	*this << "0x";
	Write(n2h((n&0xF000)>>12));
	Write(n2h((n&0x0F00)>>8));
	Write(n2h((n&0x00F0)>>4));
	Write(n2h((n&0x000F)>>0));
	return *this;
}

Console& Console::operator <<(short n) {
	return operator <<((int)n);
}

Console& Console::operator <<(uint32 n) {
	*this << "0x";
	Write(n2h((n&0xF0000000)>>28));
	Write(n2h((n&0x0F000000)>>24));
	Write(n2h((n&0x00F00000)>>20));
	Write(n2h((n&0x000F0000)>>16));
	Write(n2h((n&0x0000F000)>>12));
	Write(n2h((n&0x00000F00)>>8));
	Write(n2h((n&0x000000F0)>>4));
	Write(n2h((n&0x0000000F)>>0));
	return *this;
}

Console& Console::operator <<(int32 n) {
	if(n==0) {
		Write('0');
		return *this;
	} else {
		if(n<0) {
			Write('-');
			n = -n;
		}
		char buf[64];
		char* p = buf;
		while(n!=0) {
			*(p++) = '0'+(n%10);
			n /= 10;
		}
		while(p!=buf) {
			Write(*(--p));
		}
		return *this;
	}
}

Console& Console::operator <<(uint64 n) {
	*this << "0x";
	Write(n2h(static_cast<int>((n&0xF000000000000000)>>60)));
	Write(n2h(static_cast<int>((n&0x0F00000000000000)>>56)));
	Write(n2h(static_cast<int>((n&0x00F0000000000000)>>52)));
	Write(n2h(static_cast<int>((n&0x000F000000000000)>>48)));
	Write(n2h(static_cast<int>((n&0x0000F00000000000)>>44)));
	Write(n2h(static_cast<int>((n&0x00000F0000000000)>>40)));
	Write(n2h(static_cast<int>((n&0x000000F000000000)>>36)));
	Write(n2h(static_cast<int>((n&0x0000000F00000000)>>32)));
	Write(n2h(static_cast<int>((n&0x00000000F0000000)>>28)));
	Write(n2h(static_cast<int>((n&0x000000000F000000)>>24)));
	Write(n2h(static_cast<int>((n&0x0000000000F00000)>>20)));
	Write(n2h(static_cast<int>((n&0x00000000000F0000)>>16)));
	Write(n2h(static_cast<int>((n&0x000000000000F000)>>12)));
	Write(n2h(static_cast<int>((n&0x0000000000000F00)>>8)));
	Write(n2h(static_cast<int>((n&0x00000000000000F0)>>4)));
	Write(n2h(static_cast<int>((n&0x000000000000000F)>>0)));
	return *this;
}

Console& Console::operator <<(int64 n) {
	if(n==0) {
		Write('0');
		return *this;
	} else {
		if(n<0) {
			Write('-');
			n = -n;
		}
		char buf[64];
		char* p = buf;
		while(n!=0) {
			*(p++) = '0'+(n%10);
			n /= 10;
		}
		while(p!=buf) {
			Write(*(--p));
		}
		return *this;
	}
}

Console& Console::operator <<(float r) {
	return operator <<(*(uint32*)&r);
}

Console& Console::operator <<(double r) {
	return operator <<(*(uint64*)&r);
}

#if defined(MemoryRegion)

Console& Console::operator <<(const MemoryRegion& mem) {
	//MakeNewLine();
	for(uint i=0; i<mem.size(); ++i) {
		if(i%16==0) {
			operator <<((uint16)i);
			Write('|');
		}
		Write(' ');
		Write(n2h((mem[i]&0x000F)>>0));
		Write(n2h((mem[i]&0x00F0)>>4));
		if(i%16==15) Write(endl);
	}
	if(mem.size()%16) Write(endl);
	return *this;
}

#endif

static Console* pcon;

extern Console& getConsole() {
	return *pcon;
}

extern void setConsole(Console& c) {
	pcon = &c;
}
