#include <stdio.h>


extern "C" extern int sprintf(char* buf, const char* format, ...) {
	return -1;
}
