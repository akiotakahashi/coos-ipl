#pragma once


static inline char n2h(int n) {
	if(n<10) return '0'+n;
	return 'A'+(n-10);
}

static inline void zero(void* _p, int size) {
	byte* p = reinterpret_cast<byte*>(_p);
	while(size-->0) {
		*p = 0;
		++p;
	}
}
