using System;
using System.IO;

class FixSize {
	static int Main(string[] args) {
		if(args.Length<2) {
			Console.WriteLine("fixsize.exe size filename [tail data]");
			return 1;
		}
		try {
			int size;
			if(args[0].StartsWith("0x")) {
				size = Convert.ToInt32(args[0].Substring(2),16);
			} else {
				size = int.Parse(args[0]);
			}
			Console.WriteLine("fixsize: file size make up to {0} (0x{0:X}) bytes.", size);
			using(FileStream stream = new FileStream(args[1], FileMode.Append)) {
				int remain = size-(int)stream.Length-(args.Length-2);
				if(remain<0) {
					Console.Error.WriteLine("error: file is {0} bytes larger than specified length.", -remain);
					return 1;
				}
				stream.Write(new byte[remain], 0, remain);
				for(int i=2; i<args.Length; ++i) {
					stream.WriteByte(Convert.ToByte(args[i],16));
				}
			}
			return 0;
		} catch(Exception ex) {
			Console.WriteLine(ex);
			return 1;
		}
	}
}
