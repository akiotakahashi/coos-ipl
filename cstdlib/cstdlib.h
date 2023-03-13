#pragma once

#include <memory.h>

#if !defined(EOF)
#define EOF (-1)
#endif


extern "C" extern void abort();

extern "C" extern void memclr(void* p, size_t size);
extern "C" extern void* memcpy(void *dst, const void *src, size_t count);
extern "C" extern void* memmove(void *dst, const void *src, size_t count);

extern int stoi(const char* s);
extern int stoi(const wchar_t* s);

extern "C" extern unsigned int strlen(const char* s);
extern "C" extern unsigned int wcslen(const wchar_t* s);
extern "C" extern bool streql(const char* s1, const char* s2);
