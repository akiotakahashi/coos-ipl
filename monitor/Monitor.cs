using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CooS {

	static class Monitor {

		static int px;
		static int py;

		static unsafe void Write(char ch) {
			byte* ptr = (byte*)0xb8000;
			ptr[0 + px * 2 + py * 160] = (byte)ch;
			ptr[1 + px * 2 + py * 160] = (byte)0x7;
			++px;
		}

		static void Main(string[] args) {
			px = 74;
			py = 24;
			Write('k');
			Write('e');
			Write('r');
			Write('n');
			Write('e');
			Write('l');
		}

	}

}
