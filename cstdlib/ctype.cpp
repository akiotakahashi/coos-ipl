#include <ctype.h>


extern "C" extern int isdigit(int ch) {
	return ('0'<=ch && ch<='9');
}

extern "C" extern int isxdigit(int ch) {
	return isdigit(ch) || ('a'<=ch && ch<='f') || ('A'<=ch && ch<='F');
}

extern "C" extern int isalnum(int ch) {
	return isdigit(ch) || ('a'<=ch && ch<='z') || ('A'<=ch && ch<='Z');
}
