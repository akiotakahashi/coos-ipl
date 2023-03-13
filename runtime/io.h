#pragma once

#include "stdtype.h"
#include "stdlib.h"


extern void outp8(unsigned short port, byte data);
extern void outp16(unsigned short port, uint16 data);
extern void outp32(unsigned short port, uint32 data);

extern uint8 inp8(unsigned short port);
extern uint16 inp16(unsigned short port);
extern uint32 inp32(unsigned short port);

extern void inp16(unsigned short port, void* p, int count, unsigned short watchport, unsigned char mask, unsigned char value);


namespace IO {

	template < typename T >
	static inline void Write(unsigned short port, T value) {
		panic("This template function must be specialized.");
	}

	template<> static inline void Write(unsigned short port, byte value) {outp8(port, value);}
	template<> static inline void Write(unsigned short port, uint16 value) {outp16(port, value);}
	template<> static inline void Write(unsigned short port, uint32 value) {outp32(port, value);}

	template < typename T >
	static inline T Read(unsigned short port) {
		panic("This template function must be specialized.");
	}

	template<> static inline byte Read<byte>(unsigned short port) {return inp8(port);}
	template<> static inline uint16 Read<uint16>(unsigned short port) {return inp16(port);}
	template<> static inline uint32 Read<uint32>(unsigned short port) {return inp32(port);}

	template < typename T, typename S = T >
	class Port {
		unsigned short port;
	public:
		Port(unsigned short p) {
			port = p;
		}
	public:
		typedef S value_type;
		T read() {
			return Read<T>(port);
		}
		S operator *() {
			return Read<T>(port);
		}
	public:
		inline Port<T,S>& operator <<(const S& value) {
			const T* p = (const T*)&value;
			for(int i=0; i<sizeof(S)/sizeof(T); ++i) {
				Write<T>(port, p[i]);
			}
			return *this;
		}
	public:
		inline Port<T,S>& operator >>(S& value) {
			T* p = (T*)&value;
			for(int i=0; i<sizeof(S)/sizeof(T); ++i) {
				p[i] = Read<T>(port);
			}
			return *this;
		}
	};

}
