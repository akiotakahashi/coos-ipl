#include <string.h>

#pragma function(strcmp)
#pragma function(strlen)
#pragma function(strcat)
#pragma function(wcslen)


extern "C" extern int strcmp(const char* s1, const char* s2) {
	while(true) {
		unsigned char q1 = *(const unsigned char*)s1++;
		unsigned char q2 = *(const unsigned char*)s2++;
		if(q1<q2) return -1;
		if(q1>q2) return +1;
		if(!q1 || !q2) return (!q2-!q1);
	}
}

extern "C" extern size_t strlen(const char* s) {
	size_t l = 0;
	while(*(s++)) ++l;
	return l;
}

extern "C" extern size_t wcslen(const wchar_t* s) {
	size_t l = 0;
	while(*(s++)) ++l;
	return l;
}

extern "C" extern char* strcat(char* s1, const char* s2) {
	return strcpy(s1+strlen(s1),s2);
}

extern "C" extern int strncmp(const char* s1, const char* s2, size_t size) {
	while(size--) {
		unsigned char q1 = *(const unsigned char*)s1++;
		unsigned char q2 = *(const unsigned char*)s2++;
		if(q1<q2) return -1;
		if(q1>q2) return +1;
		if(!q1 || !q2) return (!q2-!q1);
	}
	return 0;
}

extern "C" extern char* strncpy(char* _dst, const char* src, size_t size) {
	char* dst = _dst;
	while(size--) {
		if(!(*dst=*src)) break;
	}
	return _dst;
}

extern "C" extern const char* strrchr(const char* s, int ch) {
	const char* p = NULL;
	while(*s) {
		if(*(s++)==ch) p=s;
	}
	return p;
}
