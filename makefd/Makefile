#---------------------------------------------------
#	CLIOS	makefile
#---------------------------------------------------

cygwin		= C:\cygwin\bin
as		= $(cygwin)\nasm.exe
disas		= $(cygwin)\ndisasm.exe
strip		= $(cygwin)\strip.exe
cat		= $(cygwin)\cat.exe

# for nasm
ASFLAGS		= -f bin

#---------------------------------------------------
#	suffix definitions
#---------------------------------------------------

.SUFFIXES: .asm .bin .img

#---------------------------------------------------
# コンパイルするファイルを定義
#---------------------------------------------------

target = fdimage.img
kernel = kernel.img
knlasm = kernel.asm
runtime = runtime.img

bin = fat.bin rootdirectory.bin
#drv = ..\driver\Release\driver.dll
#asm = $(bin:.bin=.asm)

#---------------------------------------------------
# 生成規則
#---------------------------------------------------

all: $(target)

$(target): ..\bootloader\bootloader.img $(bin) $(kernel) $(runtime) makefile
	-vfd.bat close
	copy /b /y ..\bootloader\bootloader.img + fat.bin + rootdirectory.bin $(target)
	..\bootloader\fixsize.exe 1474560 $(target)
	vfd.bat open $(target) /1.44 /w
	copy $(kernel) a: > nul
	copy $(runtime) a: > nul
	attrib +h +s +r a:\$(kernel)
	attrib +h +s +r a:\$(runtime)
	$(cygwin)\sleep 1s
	vfd.bat close
	vfd.bat remove

$(kernel): ..\makeknl\kernel.exe.bin
	copy /y ..\makeknl\kernel.exe.bin $@

$(runtime): ..\runtime\Release\$(runtime:.img=.dll)
	$(strip) -O binary -o $@ ..\runtime\Release\$(runtime:.img=.dll)

$(bin): $(@:.bin=.asm)

#---------------------------------------------------
#	suffix rules
#---------------------------------------------------

.asm.bin:
	$(as) $(ASFLAGS) -o $@ $<

#---------------------------------------------------
# クリア
#---------------------------------------------------

clean:
	-vfd.bat close
	-del $(bin) > nul
	-del $(kernel) > nul
	-del $(target) > nul
