#include <string.h>


extern "C" extern bool streql(const char* s1, const char* s2) {
	while(*s1 && *s1==*s2) {
		++s1;
		++s2;
	}
	return *s1==*s2;
}
