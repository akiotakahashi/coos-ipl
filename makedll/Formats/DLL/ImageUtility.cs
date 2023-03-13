using System;
using System.IO;
using System.Text;

namespace CooS.Formats.DLL {

	public abstract class ImageUtility {

		/*
		public static unsafe string GetString(sbyte* data, int start, int len, Encoding encoding) {
			byte[] data_array = new byte[len-start];
			for (int i=start; i<len; i++) {
				data_array[i-start] = (byte)*data++;
			}
			return encoding.GetString(data_array);
		}
		*/

		public static string ReadString(BinaryReader reader, int maxlength) {
			if(maxlength==0) { return null; }
			StringBuilder builder = new StringBuilder();
			byte ch;
			for(; maxlength>0; --maxlength) {
				ch = reader.ReadByte();
				if(ch<0) { throw new IOException(); }
				if(ch==0) { break; }
				builder.Append((char)ch);
			}
			return builder.ToString();
		}

	}

}
