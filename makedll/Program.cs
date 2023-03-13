using System;
using System.Collections.Generic;
using System.Text;
using CooS.Formats.DLL;
using System.IO;
using CooS.Formats;

namespace CooS {

	enum FixupTypes {
		IMAGE_REL_BASED_ABSOLUTE = 0,		/// fixupはスキップされます。このタイプはブロックを埋めるために使うことができます。
		IMAGE_REL_BASED_HIGH = 1,			/// fixupはデルタの上位16ビットをオフセットの16ビットに加えます。この16ビットフィールドは32ビット ワードの上位の値を表します。
		IMAGE_REL_BASED_LOW = 2,			/// fixupはデルタの下位16ビットをオフセットの16ビット フィールドに加えます。この16ビット フィールドは32ビット ワードの下半分を表します。このfixupはRISCマシンのために、イメージ オブジェクト アラインの値がデフォルトの64Kでない場合にだけ出力されます。
		IMAGE_REL_BASED_HIGHLOW = 3,		/// fixupは32ビットのデルタをオフセットの32ビット フィールドに適用します。上位16ビットはオフセットに置かれ、下位16ビットは次のオフセット レコードに置かれます。この2つは1つの符号付き変数に結合される必要があります。
		IMAGE_REL_BASED_HIGHADJ = 4,		/// fixupは32ビット値全部を必要とします。
		IMAGE_REL_BASED_MIPS_JMPADDR = 5,	/// MIPSのジャンプ命令に適用されるfixup。
	}

	struct FixupHeader {
		public RVA PageRVA;		/// イメージ ベースにページRVAを加えたものが各オフセットに加算されて、fixupがどこに適用される必要があるかの仮想アドレスになります。
		public uint BlockSize;	/// 後に続くタイプ/オフセットフィールドと、ページRVAおよびブロック サイズ フィールドを含めた、fixupブロックの合計バイト数。
	}

	struct FixupBlock {
		byte Type;		/// ワードの上位4ビットが格納されます。値はどのタイプのfixupが適用されるかを示します。これらのfixupsについては、fixupタイプで説明されています。
		ushort Offset;	/// ワードの残り12ビットが格納されます。開始アドレスからのオフセットは、このブロックのページRVAフィールドで指定されています。このオフセットは、fixupがどこに適用されるかを示します。
	}

	static class Program {

		public static void Main(string[] args) {
			if(!args[0].StartsWith("0x")) { throw new ArgumentException("address must start with 0x"); }
			int newadr = Convert.ToInt32(args[0].Substring(2), 16);
			string srcfile = args[1];
			string dstfile = args[2];
			File.Copy(srcfile, dstfile, true);
			using(BinaryWriter writer = new BinaryWriter(new FileStream(dstfile, FileMode.Open, FileAccess.Write))) {
				using(PEImage peimg = new PEImage(srcfile)) {
					BinaryReader reader = new BinaryReader(peimg.Stream);
					int delta = (int)(newadr - peimg.PEHeader.ImageBase);
					var reloc = peimg.Sections[".reloc"];
					Console.WriteLine("position=0x{0:X8}, totalsize={1}", reloc.PointerToRawData, reloc.VirtualSize);
					peimg.Stream.Position = reloc.PointerToRawData;
					var buf = new byte[reloc.VirtualSize];
					peimg.Stream.Read(buf, 0, buf.Length);
					using(BinaryReader scanner = new BinaryReader(new MemoryStream(buf))) {
						while(scanner.BaseStream.Position < scanner.BaseStream.Length) {
							FixupHeader hdr;
							hdr.PageRVA = scanner.ReadUInt32();
							hdr.BlockSize = scanner.ReadUInt32();
							Console.WriteLine("rva=0x{0:X8}, size={1}", hdr.PageRVA.Value, hdr.BlockSize);
							for(int i = 0; i < (hdr.BlockSize - 8) / 2; ++i) {
								ushort value = scanner.ReadUInt16();
								FixupTypes type = (FixupTypes)((value & 0xf000) >> 12);
								int offset = value & 0xfff;
								Console.WriteLine("type={0}, offset=0x{1:X8}", type, offset);
								reader.BaseStream.Position = writer.BaseStream.Position = peimg.RVAToLocation(hdr.PageRVA) + offset;
								switch(type) {
								case FixupTypes.IMAGE_REL_BASED_ABSOLUTE:
									break;
								case FixupTypes.IMAGE_REL_BASED_HIGH:
									throw new NotImplementedException();
								case FixupTypes.IMAGE_REL_BASED_LOW:
									throw new NotImplementedException();
								case FixupTypes.IMAGE_REL_BASED_HIGHLOW:
									writer.Write(reader.ReadInt32() + delta);
									break;
								case FixupTypes.IMAGE_REL_BASED_HIGHADJ:
									throw new NotImplementedException();
								case FixupTypes.IMAGE_REL_BASED_MIPS_JMPADDR:
									throw new NotSupportedException();
								}
							}
						}
					}
				}
				writer.BaseStream.Position = 0xB4;	// where IMAGE_BASE field locates
				writer.Write(newadr);
			}
		}

	}

}
