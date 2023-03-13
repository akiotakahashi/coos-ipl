#define CODE_SEGMENT	0x0120
#define DATA_SEGMENT	0x01C0
#include "bootldr.h"


#define BytesPerSector		512
#define SectorsPerFAT		9
#define SectorsPerCluster	1
#define SectorsPerTrack		18
#define ReservedSectorCount	8
#define RootEntryCount		32
#define NumberOfHeads		2
#define NumberOfFAT			2
#define NumberOfSectors		0x0B40


void printn(unsigned int n, int base);
void set32(long farptr, int offset, long value);

void clearCursor();
void clearA20();
long getMemorySizeInKb();
void initVbe(int seg, void* vbeinfo);
int getVbeInfo(int seg, void* p);
int getVbeModeInfo(int mode, int seg, void* p);
int initVbeMode(int seg, void* vbeinfo, int dstseg);
int switchVbeMode(int mode);
long loadKernel(unsigned int dstseg, const char* filename);


#define PARAM_STORE_ADDR	0x6F0

int boot() {
	long memsize;
	int vbemode;
	long size;
	prints("CooS 2nd bootloader\r\n");
	clearCursor();
	clearA20();
	memsize = getMemorySizeInKb();
	set32(0, 0x06FC, memsize);
	printn(memsize*64, 10);
	prints(" KB memory available\r\n");
	initVbe(0x0070, 0);
	vbemode = initVbeMode(0x0070, 0, 0x0080);
	size = loadKernel(0x1000, "RUNTIME IMG");
	set32(0, PARAM_STORE_ADDR+0, size);
	size = ((size+0xff)&~0xff)>>4;
	set32(0, PARAM_STORE_ADDR+4, 0x1000+size);
	size = loadKernel(0x1000+size, "KERNEL  IMG");
	set32(0, PARAM_STORE_ADDR+8, size);
	/* âÊñ êÿÇËë÷Ç¶ *
	if(vbemode>0) switchVbeMode(vbemode); else/**/
	set32(0, 0x828, NULL);
	_asm_c("db 0xEA \n dw 0x1000, 0 ;=0x0:0x1000", 0x1000);
}

void printn(unsigned int n, int base) {
	if(n==0) {
		printc('0');
	} else {
		char buf[32];
		char* p;
		p = buf;
		while(n>0) {
			*p = (n%base)+'0';
			if(*p>'9') *p=*p-'9'+'A'-1;
			++p;
			n /= base;
		}
		--p;
		while(p>=buf) {
			printc(*(p--));
		}
	}
}

void printd(int n) {
	if(n<0) {
		printc('-');
		n=-n;
	}
	printn(n, 10);
}

void printh(int n) {
	printc('0');
	printc('x');
	printn((unsigned int)n, 16);
}


char vbeinfo[512];

int get16(long farptr, int offset) {
	int s = ((int*)&farptr)[1];
	int o = (int)farptr+offset;
	return _asm_int("push ds \n mov ds, ax \n mov ax, [bx] \n pop ds", s, o);
}

void set16(long farptr, int offset, int value) {
	int s = ((int*)&farptr)[1];
	int o = (int)farptr+offset;
	_asm_int("push ds \n mov ds, ax \n mov [bx], cx \n pop ds", s, o, value);
}

long get32(long farptr, int offset) {
	int s = ((int*)&farptr)[1];
	int o = (int)farptr+offset;
	return _asm_long("push ds \n mov ds, ax \n mov ax, [bx] \n mov bx, [bx+2] \n pop ds", s, o);
}

void set32(long farptr, int offset, long value) {
	int s = ((int*)&farptr)[1];
	int o = (int)farptr+offset;
	_asm_long("push ds \n mov ds, ax \n mov [bx], cx \n mov [bx+2], dx \n pop ds", s, o, (int)value, ((int*)&value)[1]);
}

int getCodeSeg() {
	return _asm_c("mov ax, cs");
}

int getDataSeg() {
	return _asm_c("mov ax, ds");
}

int getStackSeg() {
	return _asm_c("mov ax, ss");
}

void initVbe(int seg, void* vbeinfo) {
	long l;
	int i;
	prints("GRAPHICS: ");
	getVbeInfo(seg, vbeinfo);
	l = get32(makelong(seg, (int)vbeinfo), 6);
	for(i=0; ; ++i) {
		char ch = (char)get16(l, i);
		if(ch=='\0') break;
		printc(ch);
	}
	prints("\r\n");
}

int searchMode(int seg, void* vbeinfo, int xres, int yres, int depth) {
	long l = get32(makelong(seg,(int)vbeinfo), 14);
	int i;
	for(i=0; i<64; ++i) {
		char buf[256];
		int mode = get16(l,i*2);
		if(mode==0xFFFF) break;
		if(!getVbeModeInfo(mode, getStackSeg(), buf)) {
			prints("VBE MODE INFO FAILED\r\n");
		} else {
			short x = *(short*)&buf[18];
			short y = *(short*)&buf[20];
			char d = buf[25];
			if(x>0 && y>0 && d>0
			&& (xres<=0 || x==xres)
			&& (yres<=0 || y==yres)
			&& (depth<=0 || d>=depth))
			{
				return mode;
			}
		}
	}
	return -1;
}

int initVbeMode(int seg, void* vbeinfo, int dstseg) {
	long l;
	int mode;
	l = get32(makelong(seg,(int)vbeinfo), 14);
	mode = -1;
	if(mode<0) mode=searchMode(seg, vbeinfo, 1152, 864, 24);
	if(mode<0) mode=searchMode(seg, vbeinfo, 1280, 1024, 24);
	if(mode<0) mode=searchMode(seg, vbeinfo, 1024, 768, 24);
	if(mode<0) mode=searchMode(seg, vbeinfo, 800, 600, 24);
	if(mode<0) mode=searchMode(seg, vbeinfo, 1152, 864, 16);
	if(mode<0) mode=searchMode(seg, vbeinfo, 1280, 1024, 16);
	if(mode<0) mode=searchMode(seg, vbeinfo, 1024, 768, 16);
	if(mode<0) mode=searchMode(seg, vbeinfo, 800, 600, 16);
	if(mode<0) mode=searchMode(seg, vbeinfo, 1152, 0, 24);
	if(mode<0) mode=searchMode(seg, vbeinfo, 1280, 0, 24);
	if(mode<0) mode=searchMode(seg, vbeinfo, 1024, 0, 24);
	if(mode<0) mode=searchMode(seg, vbeinfo, 800, 0, 24);
	if(mode<0) mode=searchMode(seg, vbeinfo, 1152, 0, 16);
	if(mode<0) mode=searchMode(seg, vbeinfo, 1280, 0, 16);
	if(mode<0) mode=searchMode(seg, vbeinfo, 1024, 0, 16);
	if(mode<0) mode=searchMode(seg, vbeinfo, 800, 0, 16);
	if(mode<0) mode=searchMode(seg, vbeinfo, 640, 480, 24);
	if(mode<0) mode=searchMode(seg, vbeinfo, 640, 480, 16);
	if(mode<0) mode=searchMode(seg, vbeinfo, 640, 0, 24);
	if(mode<0) mode=searchMode(seg, vbeinfo, 640, 0, 16);
	if(mode==-1) {
		prints("SUITABLE VBE MODE NOT FOUND");
		freeze();
		return -1;
	} else {
		char buf[256];
		if(!getVbeModeInfo(mode, getStackSeg(), buf) || !getVbeModeInfo(mode, dstseg, (void*)0)) {
			prints("VBE MODE INFO FAILED\r\n");
			freeze();
		} else {
			short xres = *(short*)&buf[18];
			short yres = *(short*)&buf[20];
			char depth = buf[25];
			prints("MODE: [");
			printn(mode, 16);
			prints("] ");
			printd(xres);
			prints("x");
			printd(yres);
			prints("x");
			printd(depth);
			prints("\r\n");
		}
		return mode;
	}
}

long loadKernel(unsigned int dstseg, const char* filename) {
	int i;
	char pfat[SectorsPerFAT*BytesPerSector];
	char proot[RootEntryCount*32];
	read(getStackSeg(), (int)pfat, 1+ReservedSectorCount+SectorsPerFAT, SectorsPerFAT);
	read(getStackSeg(), (int)proot, 1+ReservedSectorCount+SectorsPerFAT*NumberOfFAT, (RootEntryCount*32)/512);
	for(i=0; i<RootEntryCount*32; i+=32) {
		int j, flag;
		if(proot[i]=='\0') continue;
		flag = 0;
		for(j=0; j<11; ++j) {
			if(proot[i+j]!=filename[j]) {
				flag = 1;
				break;
			}
		}
		if(flag==0) {
			short cluster = *(short*)&proot[i+26];
			long size = *(long*)&proot[i+28];
			long copyed = 0;
			unsigned int dstoff;
			prints("READ ");
			printh(size>>16);
			prints(" ");
			printh((int)size);
			prints(" BYTES FROM #");
			printd(cluster);
			prints(" INTO 0x");
			printh((int)dstseg);
			prints(":0\r\n");
			dstoff = 0;
			while(size>copyed) {
				unsigned short t;
				prints("\rREAD #");
				printd(cluster);
				prints("...");
				printh((int)dstseg);
				prints("/");
				printh((int)dstoff);
				prints("\r");
				read(dstseg, dstoff, 1+ReservedSectorCount+SectorsPerFAT*NumberOfFAT+(RootEntryCount*32)/512+cluster-2, 1);
				t = *(unsigned short*)(pfat+cluster*12/8);
				if(cluster&1) {
					t >>= 4;
				} else {
					t &= 0x0FFF;
				}
				cluster = t;
				copyed += 512;
				if(dstoff>=0xFFFF-512) {
					dstseg += 0x1000;
					dstoff = 0;
				} else {
					dstoff += 512;
				}
			}
			prints("\rLOADING IMAGE COMPLETED\r\n");
			return size;
		}
	}
	prints("BOOTSTRAP IMAGE NOT FOUND");
	freeze();
}

void clearCursor() {
	/* ÉJÅ[É\ÉãÇè¡ãé */
	_asm_c(
		"mov	ah, 0x01	\n"
		"mov	ch, 0x20	\n"
		"int	0x10		\n"
		);
}

void clearA20() {
	_asm_c("int 0x15", 0x2401);
	prints("Enabled A20-LINE\r\n");
}

long getMemorySizeInKb() {
	/*
	CF	Carry Flag	Non-Carry - indicates no error
	AX	Extended 1	Number of contiguous KB between 1 and 16 MB,
				maximum 0x3C00 = 15 MB.
	BX	Extended 2	Number of contiguous 64 KB blocks between
				16 MB and 4 GB.
	CX	Configured 1	Number of contiguous KB between 1 and 16 MB,
				maximum 0x3C00 = 15 MB.
	DX	Configured 2	Number of contiguous 64 KB blocks between
				16 MB and 4 GB.
	*/
/*
	int l = _asm_c("int 0x15	\n	mov ax, cx", 0xE801, 0, 0, 0);
	int h = _asm_c("int 0x15	\n	mov ax, dx", 0xE801, 0, 0, 0);
*/
	int l = _asm_c("int 0x15	\n", 0xE801, 0, 0, 0);
	int h = _asm_c("int 0x15	\n	mov ax, bx", 0xE801, 0, 0, 0);
	return h*64 + l;
}

int getVbeInfo(int seg, void* p) {
	int ret = _asm_c(
		"	push di			\n"
		"	mov	es, bx		\n"
		"	mov	di, ax		\n"
		"	mov	ax, 0x4F00	\n"
		"	int	0x10		\n"
		"	pop di			\n"
		, p, seg);
	return ret==0x004F;
}

int getVbeModeInfo(int mode, int seg, void* p) {
	int ret = _asm_c(
		"	push di			\n"
		"	mov	es, bx		\n"
		"	mov	di, ax		\n"
		"	mov	ax, 0x4F01	\n"
		"	int	0x10		\n"
		"	pop di			\n"
		, p, seg, mode);
	return ret==0x004F;
}

int switchVbeMode(int mode) {
	int ret = _asm_c(
		"	or	bx, 0x4000	\n"
		"	int	0x10		\n"
		, 0x4F02, mode);
	return ret==0x004F;
}
