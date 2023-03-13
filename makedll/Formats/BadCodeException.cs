using System;

namespace CooS.Formats {

	public class BadCodeException : SystemException {
	
		public BadCodeException() {
		}

		public BadCodeException(string msg) : base(msg) {
		}

	}

}
