;-----------------------------------------------------------------------------

%define ORG 0x00007C00

org ORG ; binary image loaded by BIOS at 07C0:0000

bits 16 ; startup is done in real mode

;-----------------------------------------------------------------------------
boot:
	xor	ax,ax		; set data segments to 0
	mov	ds,ax
	mov	es,ax
	mov	fs,ax
	mov	gs,ax

	mov	ax,0x9000	; create stack under segment 0xA000
	mov	ss,ax
	mov	sp,0xFFF0

	xor	ah,ah
	int	0x16		; await keypress

	mov	ax,0x0003	; switch to 80x25
	int	0x10

	xor	ah,ah
	int	0x16		; await keypress

	mov	si,msg1		; display msg1 [ds:si]
.l1
	lodsb
	or	al,al
	jz	.e1
	mov	ah,0x0E		; BIOS teletype
	mov	bh,0x00		; display page 0
	mov	bl,0x07		; text attribute
	int	0x10		; invoke BIOS
	jmp	.l1
.e1
	xor	ah,ah
	int	0x16		; await keypress

	mov	ax,0xB800
	mov	es,ax		; write to [es:di]
	mov	di,((80*5+8)*2)
	mov	si,msg2		; display msg2 [ds:si]
.l2
	lodsb
	or	al,al
	jz	.e2
	stosb
	inc	edi
	jmp	.l2
.e2
	xor	ah,ah
	int	0x16		; await keypress

	int	0x19		; warm boot computer

msg1	db 0x0D,0x0A,"Bios message !",0x0D,0x0A,0x00
msg2	db "Raw message !",0x00

;-----------------------------------------------------------------------------
	times 510-($-$$) db 0	; the boot sector (512 bytes) must end with
	db 0x55, 0xAA		; 55 AA to be loaded by BIOS
;-----------------------------------------------------------------------------
