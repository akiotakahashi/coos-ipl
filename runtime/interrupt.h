#pragma once

#include "stdtype.h"


typedef void (__stdcall INTERRUPT_HANDLER)();

#define _InterruptHandler_Prologue \
	__asm { mov ebp, esp } \
	__asm { sub esp, __LOCAL_SIZE }

#define _InterruptHandler_Epilogue \
	__asm { mov esp, ebp }

#define InterruptHandler_Prologue \
	__asm { pushad } \
	_InterruptHandler_Prologue

#define InterruptHandler_Epilogue \
	_InterruptHandler_Epilogue \
	__asm { popad } \
	__asm { iretd }


namespace Interrupt {

	extern void Initialize();
	extern void RegisterInterruptGate(byte intno, INTERRUPT_HANDLER* handler);
	extern void RegisterTrapGate(byte intno, INTERRUPT_HANDLER* handler);

}
