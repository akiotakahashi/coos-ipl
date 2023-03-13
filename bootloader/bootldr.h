int _asm_c();
int _asm_int();
long _asm_long();

#define _enstring_(s)  # s
#define combine(a,b) (((int)(a)<<8)|(b))
#define makelong(a,b) (((long)(a)<<16)|(b))

#define true 1
#define false 0
#define NULL 0

int boot();
void freeze();

int main() {
	_asm_c(
		"	db	0xEA      \n"
		"	dw	0x0005    \n"
		"	dw	" _enstring_(__eval__(CODE_SEGMENT)) "	\n"
		);
	_asm_c(
		"	mov ax, " _enstring_(__eval__(DATA_SEGMENT)) "	\n"
		"	mov	ds, ax    \n"
		"	mov	es, ax    \n"
		);
	boot();
	_asm_c("retf");
}

void freeze() {
	for(;;) {
		_asm_c("hlt");
	}
}

void printc(char ch) {
	_asm_c(
		"mov	ah, 0x0E  \n"
		"mov	bx, 7     \n"
		"int	10h       \n"
		, ch);
}

void prints(const char* s) {
	while(*s) {
		printc(*(s++));
	}
}

int read(int segment, int offset, int sector, int count) {
	int track, head, cylinder;
	--sector;
	track = sector/18;
	head = track&1;
	cylinder = track>>1;
	sector %= 18;
	++sector;
	_asm_c("mov	es, ax", segment);	/* Segment of buffer */
	head = _asm_c(
		"int	13h			\n"		/* Diskette services */
		"mov	bx, ds		\n"
		"mov	es, bx		\n"
		, /*AX*/ 0x0200|count				/* Read diskette sectors */	/* Number of sectors (1-15) */
		, /*BX*/ offset						/* Offset of buffer */
		, /*CX*/ combine(cylinder,sector)	/* Track number (0-79) */	/* Sector number (8-36) */
		, /*DX*/ combine(head,0)			/* Head number (0-1) */		/* Drive number (0-1) */
		);
	return head;
}
