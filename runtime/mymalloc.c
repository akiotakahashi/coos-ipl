#define HAVE_MMAP			0
#define MSPACES				1
#define ONLY_MSPACES		1
#define NO_MALLIFO			1
#define MORECORE_CONTIGUOUS	0
#define DEBUG				1
#define malloc_getpagesize	4096
#define ABORT				abort()
#define MALLOC_FAILURE_ACTION

__declspec(noreturn) void abort();
#define _CRT_TERMINATE_DEFINED


// fprintf を置き換える。

#include <stdio.h>

#define fprintf myfprintf
static void fprintf(FILE* file, const char* format, ...) {
}


// malloc を取り込む。

#undef WIN32
#undef _WIN32

#define LACKS_UNISTD_H
#define LACKS_FCNTL_H
#define LACKS_SYS_PARAM_H
#define LACKS_SYS_MMAN_H
#define LACKS_STRINGS_H
#define LACKS_STRING_H
#define LACKS_SYS_TYPES_H
#define LACKS_ERRNO_H
#define LACKS_STDLIB_H

#pragma warning(disable: 4146)
#include "malloc.c"


// __imp____iob_func が未定義になるので定義してしまう。

extern int _imp____iob_func() {
}
