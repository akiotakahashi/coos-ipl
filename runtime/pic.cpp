#include "pic.h"
#include "io.h"


namespace PIC {

	class PICUnit {
		uint16 port;
	public:
		PICUnit() {
		}
	public:
		void Initialize(uint16 baseport, byte int_vector_base, byte icw3) {
			port = baseport;
			outp8(port, 0x10 | 0x01);	/* specify ICW1 and need ICW4*/
			outp8(port+1, int_vector_base);
			outp8(port+1, icw3);
			outp8(port+1, 0x01);
		}
		void setOCW1(byte value) {
			outp8(port+1, value);
		}
		void setOCW2(byte value) {
			outp8(port, value);
		}
		void setOCW3(byte value) {
			outp8(port, value|0x08);
		}
	};


	static PICUnit mas;
	static PICUnit sla;
	static byte base_mas;
	static byte base_sla;
	static byte mask_mas;
	static byte mask_sla;


	extern void Initialize() {
		mas.Initialize(0x0020, base_mas=0x60, 0x04);
		sla.Initialize(0x00A0, base_sla=0x68, 0x02);
		mas.setOCW1(mask_mas = (0xFF & ~0x04));
		sla.setOCW1(mask_sla = (0xFF));
	}

	extern void EnableInterrupt(byte irq) {
		if(irq<8) {
			irq = 1<<irq;
			mask_mas &= ~irq;
			mas.setOCW1(mask_mas);
		} else {
			irq = 1<<(irq-8);
			mask_sla &= ~irq;
			sla.setOCW1(mask_sla);
		}
	}

	extern void DisableInterrupt(byte irq) {
		if(irq<8) {
			irq = 1<<irq;
			mask_mas |= irq;
			mas.setOCW1(mask_mas);
		} else {
			irq = 1<<(irq-8);
			mask_sla |= irq;
			sla.setOCW1(mask_sla);
		}
	}

	extern void NotifyEndOfInterrupt(byte irq) {
		if(irq<8) {
			mas.setOCW2(0x20);
		} else {
			mas.setOCW2(0x20);
			sla.setOCW2(0x20);
		}
	}

	extern void RegisterInterruptHandler(byte irq, INTERRUPT_HANDLER* handler) {
		if(irq<8) {
			irq += base_mas;
		} else {
			irq += base_sla-8;
		}
		Interrupt::RegisterInterruptGate(irq, handler);
	}

}
