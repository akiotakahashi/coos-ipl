%include "define.mac"

%rep	RootEntryCount
	db	0x00		; Indicates that this is free.
	db	'          '	; Dir Name
	db	0		; Attributes
	db	0		; for WindowsNT
	db	0		; Creation Time Tenth
	dw	0		; Creation Time
	dw	0		; Creation Date
	dw	0		; Last Access Date
	dw	0		; First Cluster (high)
	dw	0		; Write Time
	dw	0		; Write Date
	dw	0		; First Cluster (low)
	dd	0		; File Size
%endrep
