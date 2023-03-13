%define BytesPerSector		512
%define SectorsPerFAT		9
%define SectorsPerCluster	1
%define SectorsPerTrack		18
%define ReservedSectorCount	8
%define RootEntryCount		32
%define NumberOfHeads		2
%define NumberOfFAT		2
%define NumberOfSectors		0x0b40

%define RootDirSectors		((RootEntryCount*32) + (BytesPerSector-1)) / BytesPerSector
%define FirstDataSector		ReservedSectorCount + (NumberOfFAT * SectorsPerFAT) + RootDirSectors

[bits 16]
[org 0]
	jmp	short start
	nop

; boot parameter block(bpb)
oem	db	"CLIOS   "
bps	dw	BytesPerSector			; Bytes per Sector
spc	db	SectorsPerCluster		; Sectors per Cluster
rsc	dw	ReservedSectorCount		; Reserved sector count: spec dictates this should be 1.
nof	db	NumberOfFAT			; Number of FAT
rec	dw	RootEntryCount			; Root Entry Count
nos	dw	NumberOfSectors			; Total sector count
med	db	0xf0				; MediaType
spf	dw	SectorsPerFAT			; Sectors per FAT
spt	dw	SectorsPerTrack			; Sectors per Track
noh	dw	NumberOfHeads			; Number of heads
	dd	0
	dd	0
	db	0
	db	0
	db	0x29
	dd	0x00000000
	db	"clios boot "
	db	"FAT12   "

; ブートローダースタート
start:
	mov	sp, 0x4000
	mov	ax, 0x0200
	mov	ss, ax
	call	0x07C0:firstboot
	call	0x0120:0x0000
	call	0x0100:0x0000

;0で埋める
	times 0x60-($-$$) db 0
firstboot:
	; COMBINED WITH 1stboot.c
