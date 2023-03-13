#---------------------------------------------------
#	CooS makefile
#---------------------------------------------------

target = $(TargetDir)\..\coos.iso
fdimg = ..\makefd\fdimage.img

#---------------------------------------------------
# ê∂ê¨ãKë•
#---------------------------------------------------

all: $(target)

$(target): $(fdimg)
	-mkdir $(TargetDir)
	copy /y $(fdimg) $(TargetDir)\bootldr.img > nul
	genisoimage.exe -verbose -iso-level 3 -eltorito-boot bootldr.img -o $(target) $(TargetDir)

#---------------------------------------------------
# ÉNÉäÉA
#---------------------------------------------------

clean:
	-del $(target)
	-rmdir /S /Q $(TargetDir)
