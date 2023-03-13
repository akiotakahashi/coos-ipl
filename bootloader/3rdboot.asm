[ORG 0x00001000]

;--------------------------------------------------------
; プロテクトモードへ移行
;--------------------------------------------------------
[BITS 16]
enter_protectmode:
	mov	si, ax		; AX = segment of kernel.img
	mov	ax, cs
	mov	ds, ax
	mov	es, ax
	cli
	lgdt	[gdtr]
	mov	eax, cr0
	or	eax, 1
	mov	cr0, eax	; プロテクトモードへ
	jmp	near far_jump
far_jump:
	jmp	dword 0x08:on_protectmode

;********************************************************
; 32 bit コード
;********************************************************

[bits 32]
on_protectmode:
	mov	ax, 0x10		; ds & es selector
	mov	ds, ax			; is 0x10
	mov	es, ax			;
	mov	ax, 0x18		; ss selector
	mov	ss, ax			; is 0x18
	mov	ax, 0x00		; null gdt
	mov	fs, ax
	mov	gs, ax
	lidt	[idtr+0x10000]		; IDTの設定
	mov	esp, 1024*1024*4	; sp is 4MB
	; カーネルコードのコピー
	and	esi, 0x0000FFFF		; SI indicates segment of runtime.img
	shl	esi, 4
	mov	edi, 0x00101000		; codebase of runtime.img
	mov	ecx, [0x06F0+0]		; ECX = size of runtime.img
	cld
	rep	movsb
	; kernel.img
	mov	esi, [0x06F0+4]		; ESI = beginning address of kernel.img
	shl	esi, 4
	mov	edi, 0x00201000		; codebae of kernel.img
	mov	ecx, [0x06F0+8]		; ECX = size of kernel.img
	cld
	rep	movsb
	; 1ch
	mov	eax, [lastmsg]
	mov	[0xb8000], eax
	;------------------------------------------------
	; CR4の操作をここでする
	; これによってSSE/SSE2が許可される
	; 未対応のCPUでこれするとどうなるかは不明
	mov	eax, cr4
	or	eax, 0x200
	mov	cr4, eax
	;------------------------------------------------
	; カーネルにジャンプ
	lss	esp, [stack]
;	jmp	stop
	jmp	0x8:0x101000

stack	dd	0x00400000
	dw	0x0018

;--------------------------------------------------------
; 処理の停止
;--------------------------------------------------------
stop:
	cli
forever:
	hlt
	jmp	short forever

;--------------------------------------------------------
;	GDT definitions
;--------------------------------------------------------
gdtr:
	dw	gdt_end-gdt0-1  	; gdt limit
	dd	gdt0

gdt0:			; segment 00
	dw 0			; segment limitL
	dw 0			; segment baseL
	db 0			; segment baseM
	db 0			; segment type
	db 0			; segment limitH, etc.
	db 0			; segment baseH
gdt08:			; segment 08(code segment)
	dw 0xffff		; segment limitL
	dw 0x0000		; segment baseL
	db 0			; segment baseM
	db 0x9a 		; Type Code
	db 0xdf 		; segment limitH, etc.
	db 0			; segment baseH
gdt10:			; segment 10(data segment)
	dw 0xffff		; segment limitL
	dw 0x0000		; segment baseL
	db 0			; segment baseM
	db 0x92 		; Type Data
	db 0xdf 		; segment limitH, etc.
	db 0			; segment baseH
gdt18:			; segment 18(stack segment)
	dw 0x20			; segment limitL (2MB)
	dw 0x0000		; segment baseL
	db 0			; segment baseM
	db 0x96 		; Type Stack
	db 0xC0 		; segment limitH, etc.
	db 0			; segment baseH

gdt_end:			; end of gdt

;--------------------------------------------------------
;	IDT definitions
;--------------------------------------------------------
idtr:
	dw	idt_end-idt_begin-1	; IDTのサイズ
	dd	0x10000+idt_begin	; IDTのアドレス

%macro	makeidt	1
	dw	ig%1
	dw	0x08
	db	0
	db	2+4+8+0+0x80
	dw	0x0001
%endmacro

idt_begin:
%assign	i 0
%rep	2
	makeidt	i
%assign	i i+1
%endrep
idt_end:

;--------------------------------------------------------
;	Interrupt Handlers
;--------------------------------------------------------

%macro	makeig	1
ig%1:
	mov	ebx, eax
	mov	eax, %1
	sti
igb%1:
	;hlt
	jmp	igb%1
	iret
%endmacro

%assign	i 0
%rep	2
	makeig	i
%assign	i i+1
%endrep

;--------------------------------------------------------
;	メッセージ
;--------------------------------------------------------

lastmsg		db	'>', 0x0F
		db	'>', 0x0F

times 512 - ($-$$) db 0
