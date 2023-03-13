REM @ECHO OFF
SET QEMU=D:\\misc\\qemu-0.9.1-windows
%QEMU%\qemu.exe -L %QEMU% -m 32 -boot d -cdrom %1\\coos.iso
