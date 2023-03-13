#define busyloop for(;;) { __asm { nop } __asm { nop } }


extern void kernelmain();

extern "C" extern void __stdcall entrypoint(int argc, char* argv[], char* envp[]);


#pragma alloc_text(".entry", entrypoint)

#pragma optimize("", off)

extern "C" extern __declspec(naked) void __stdcall entrypoint(int argc, char* argv[], char* envp[]) {
	__asm jmp kernelmain;
}

#pragma optimize("", on)
