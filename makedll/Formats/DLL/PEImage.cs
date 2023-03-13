
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
// 
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
/*
 * Copyright (c) 2002 Sergey Chaban <serge@wildwestsoftware.com>
 */

using System;
using System.IO;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace CooS.Formats.DLL {

	public class PEImage : IDisposable {

		readonly DOSHeader dosHdr = new DOSHeader();
		readonly COFFHeader coffHdr = new COFFHeader();
		readonly PEHeader peHdr = new PEHeader();
		readonly Dictionary<string, Section> sections = new Dictionary<string, Section>();

		readonly string filepath;
		readonly Stream stream;
		BinaryReader reader;

		public PEImage(string name) {
			this.filepath = name;
			FileInfo pe = new FileInfo(name);
			this.stream = new BufferedStream(pe.OpenRead());
			this.reader = new BinaryReader(stream);
			ReadHeaders();
			ReadSections();
		}

		public PEImage(Stream stream) {
			this.filepath = null;
			this.stream = stream;
			this.reader = new BinaryReader(stream);
			ReadHeaders();
			ReadSections();			
		}

		~PEImage() {
			Close();
		}

		private void ReadHeaders() {
			if(reader==null) { throw new Exception("You must open image before trying to read it."); }
			dosHdr.Read(reader);
			reader.BaseStream.Position = dosHdr.Lfanew;
			ExeSignature peSig;
			peSig = (ExeSignature) reader.ReadUInt16();
			if(peSig!=ExeSignature.NT) { throw new BadImageFormatException("Invalid image format: cannot find PE signature."); }
			peSig = (ExeSignature) reader.ReadUInt16();
			if(peSig!=ExeSignature.NT2) { throw new BadImageFormatException("Invalid image format: cannot find PE signature."); }
			coffHdr.Read(reader);
			peHdr.Read(reader);
		}

		private void ReadSections() {
			int n = coffHdr.NumberOfSections;
			for(int i = n; --i >=0; ) {
				Section sect = new Section();
				sect.Read(reader);
				sections[sect.Name] = sect;
			}
		}

		public Stream Stream {
			get {
				return this.stream;
			}
		}

		public COFFHeader CoffHeader {
			get {
				return this.coffHdr;
			}
		}

		public PEHeader PEHeader {
			get {
				return this.peHdr;
			}
		}

		public byte[] GenerateImage() {
			byte[] image = new byte[peHdr.SizeOfImage];
			foreach(Section s in sections.Values) {
				this.stream.Position = s.PointerToRawData;
				// VirtualSize < SizeOfRawData ‚Ìê‡‚ ‚èB
				int actualsize = Math.Min((int)s.VirtualSize, (int)s.SizeOfRawData);
				//Console.WriteLine("Read section into 0x{0:X} with {1} bytes and clear {2} bytes", s.VirtualAddress.Value, actualsize, s.VirtualSize-actualsize);
				this.stream.Read(image, (int)s.VirtualAddress.Value, actualsize);
				Array.Clear(image, (int)(s.VirtualAddress.Value+s.SizeOfRawData), (int)s.VirtualSize-actualsize);
			}
			return image;
		}

		public Dictionary<string,Section> Sections {
			get {
				return sections;
			}
		}

		public void Close() {
			lock (this) {
				if(reader!=null) {
					reader.Close();
				}
			}
		}

		// IDisposable
		public void Dispose() {
			Close();
		}

		public bool IsCLI {
			get {
				return peHdr.IsCLIImage;
			}
		}

#if false
		internal CorHeader ReadCorHeader() {
			if(!this.IsCLI) throw new NotSupportedException();
			reader.BaseStream.Position = RVAToVA(peHdr.CLIHdrDir.virtAddr);
			return new CooS.Formats.CLI.Metadata.CorHeader(this, stream);
		}
#endif

		public void WriteHeaders (BinaryWriter writer) {
			dosHdr.Write (writer);
			writer.BaseStream.Position = dosHdr.Lfanew;
			writer.Write ((ushort)ExeSignature.NT);
			writer.Write ((ushort)ExeSignature.NT2);
			
			coffHdr.Write (writer);
			peHdr.Write (writer);
		
			WriteSections (writer);
			
		}

		protected void WriteSections (BinaryWriter writer) {
			foreach (Section section in sections.Values) {
				section.Write (writer);
			}
		}


		/// <summary>
		/// </summary>
		/// <param name="writer"></param>
		public void Dump(TextWriter writer) {
			writer.WriteLine("COFF Header:");
			writer.WriteLine(coffHdr.ToString());
			writer.WriteLine("PE Header:");
			writer.WriteLine(peHdr.ToString());
		}


		/// <summary>
		/// </summary>
		/// <returns></returns>
		public override string ToString() {
			StringWriter sw = new StringWriter();
			Dump(sw);
			return sw.ToString();
		}


		/// <summary>
		///  Returns name of the section for the given RVA.
		/// </summary>
		/// <param name="rva"></param>
		/// <returns></returns>
		public Section RVAToSection(RVA rva) {
			foreach(Section s in Sections.Values) {
				RVA sva = s.VirtualAddress;
				if(sva<=rva && rva<sva+s.SizeOfRawData) {
					return s;
				}
			}
			return null;
		}

		public long RVAToLocation(RVA rva) {
			Section s = RVAToSection(rva);
			if(s==null) { throw new ArgumentOutOfRangeException(); }
			return (rva.Value-s.VirtualAddress)+s.PointerToRawData;
		}

	}

}

