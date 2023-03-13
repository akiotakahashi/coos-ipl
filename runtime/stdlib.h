#pragma once

#include "../cstdlib/cstdlib.h"


extern void dump(const char* msg);
extern __declspec(noreturn) void panic(const char* msg);
extern __declspec(noreturn) void panic(const wchar_t* msg);
inline static void assert(bool cond, const char* msg) { if(!cond) panic(msg); }

#define malloc mymalloc
#define free myfree

extern "C" extern void* malloc(size_t size);
extern "C" extern void free(void* p);
