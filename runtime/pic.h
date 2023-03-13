#pragma once

#include "stdtype.h"
#include "interrupt.h"


namespace PIC {

	extern void Initialize();
	extern void EnableInterrupt(byte irq);
	extern void DisableInterrupt(byte irq);
	extern void NotifyEndOfInterrupt(byte irq);
	extern void RegisterInterruptHandler(byte irq, INTERRUPT_HANDLER* handler);

}
