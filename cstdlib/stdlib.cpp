
extern int stoi(const char* s) {
	int n = 0;
	bool minus = false;
	const char* p = s;
	if(p[0]=='-') {
		minus = true;
		++p;
	}
	while(true) {
		char x = *(p++)-'0';
		if(x<0 || 9<x) break;
		n *= 10;
		n += x;
	}
	return minus ? -n : n;
}

extern int stoi(const wchar_t* s) {
	int n = 0;
	bool minus = false;
	const wchar_t* p = s;
	if(p[0]=='-') {
		minus = true;
		++p;
	}
	while(true) {
		char x = *(p++)-'0';
		if(x<0 || 9<x) break;
		n *= 10;
		n += x;
	}
	return minus ? -n : n;
}

extern "C" extern long atol(const char* s) {
	return (long)stoi(s);
}
