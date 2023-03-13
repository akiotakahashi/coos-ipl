#---------------------------------------------------
#	CooS makefile
#---------------------------------------------------

source = ..\monitor\bin\Release\kernel.exe
target = kernel.exe.bin

#---------------------------------------------------
# ê∂ê¨ãKë•
#---------------------------------------------------

all: $(target)

$(target): kernel.exe.dll sectiondump.exe
	nm kernel.exe.dll | sort > kernel.exe.map
	nm kernel.exe.dll | sort | sectiondump.exe > kernel.exe.h
	objcopy -O binary -S kernel.exe.dll $@

kernel.exe.dll: $(source)
	mono --aot $(source)
	..\makedll\bin\Release\makedll.exe 0x00200000 $(source).dll kernel.exe.dll

sectiondump.exe: sectiondump.cs
	csc sectiondump.cs
	
#---------------------------------------------------
# ÉNÉäÉA
#---------------------------------------------------

clean:
	-del $(target)
