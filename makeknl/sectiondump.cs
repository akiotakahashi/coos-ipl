using System;

class T {
	static void Main(string[] args) {
		string line;
		while((line=Console.ReadLine())!=null) {
			string[] parts = line.Split(' ');
			Console.WriteLine("#define SECTION_ADDR_{0} 0x{1}"
				, parts[2]
					.Replace('.','_')
					.Replace('@','_')
					.Replace('$','_')
				, parts[0]);
		}
	}
}
