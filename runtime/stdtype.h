#pragma once


#define NULL 0


typedef unsigned char byte;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

typedef char int8;
typedef short int16;
typedef long int32;
typedef long long int64;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32;
typedef unsigned long long uint64;

// native int/uint
#if defined(BIT64)
typedef int64 nint;
typedef uint64 unint;
#else
typedef int32 nint;
typedef uint32 unint;
#endif

//typedef unsigned short wchar_t;
typedef wchar_t wchar;


//#define _PROPERTY(type, name) inline type get ## name()
#define PROPERTY(type, name) __declspec(property(get=get ## name)) type name; inline type get ## name()
#define INDEXER(type, name, itype) PROPERTY(type,name) const {panic("INVAILD INDEXER");} inline type get ## name(itype index)


enum KeyCode {
	KEY_NONE,
	KEY_ESC,
	KEY_BACKSPACE,
	KEY_TAB,
	KEY_RETURN,
	KEY_LCTRL,
	KEY_LSHIFT,
	KEY_RSHIFT,
	KEY_LALT,
	KEY_CAPSLOCK,
	KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_PAD_NUMLOCK,
	KEY_SCRLOCK,
	KEY_PAD_7,
	KEY_PAD_8,
	KEY_PAD_9,
	KEY_PAD_MINUS,
	KEY_PAD_4,
	KEY_PAD_5,
	KEY_PAD_6,
	KEY_PAD_PLUS,
	KEY_PAD_1,
	KEY_PAD_2,
	KEY_PAD_3,
	KEY_PAD_0,
	KEY_PAD_PERIOD,
	KEY_F11,
	KEY_F12,
	KEY_PAD_ENTERKEY_RCTRL,
	KEY_PAD_DIVIDE,
	KEY_PRTSCRN,
	KEY_RALT,
	KEY_HOME,
	KEY_ARROW_UP,
	KEY_PAGEUP,
	KEY_ARROW_LEFT,
	KEY_ARROW_RIGHTKEY_END,
	KEY_ARROW_DOWNKEY_PGDNKEY_INS,
	KEY_LWIN,
	KEY_RWIN,
	KEY_MENU,
	KEY_RCTRL,
	KEY_ARROW_RIGHT,
	KEY_END,
	KEY_ARROW_DOWN,
	KEY_PAGEDOWN,
	KEY_INS,
	KEY_PAD_ENTER,
	//
	KEY_0,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_A,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,
	KEY_HYPHEN,		// -
	KEY_CARET,		// ^
	KEY_ATMARK,		// @
	KEY_BRACKET_L,	// [
	KEY_SEMICOLON,	// ;
	KEY_COLON,		// :
	KEY_BRACKET_R,	// ]
	KEY_BRACKET_R2,	// ]
	KEY_COMMA,		// ,
	KEY_PERIOD,		// .
	KEY_SLASH,		// /
	KEY_ASTERISK,	// *
	KEY_SPACE,		// ' '
};

#define halt __asm { sti } for(;;) { __asm { hlt } }
#define freeze __asm { cli } {ushort*p=(ushort*)0xb8000;p[0]='F'|0xF00;p[1]='Z'|0xF00;} for(;;) { __asm { hlt } }
