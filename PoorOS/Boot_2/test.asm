;-----------------------------------------------------------------------------

msg1	db	0x0D,0x0A,"Hello Flat Real World !",0x0D,0x0A,0x00
msg2	db	"Raw message !",0x00
msg3	db	"-=-=-=-=-=-=-",0x00

;-----------------------------------------------------------------------------

simpleTest:
	push	bp
	mov	bp,sp

	mov	esi,msg1	; display msg1 [ds:esi]
.l1
	a32 lodsb
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

	mov	edi,0x000B8000	; write to [es:edi]
	mov	esi,msg2	; display msg2 [ds:esi]
.l2
	a32 lodsb
	or	al,al
	jz	.e2
	a32 stosb
	inc	edi
	jmp	.l2
.e2
	xor	ah,ah
	int	0x16		; await keypress


	mov	ax,0x0013	; switch to 320x200x256
	int	0x10

	mov	ebx,200
.g2
	or	ebx,ebx
	jz	.ge2
	dec	ebx
	mov	edi,0x000A0000	; graphic output
	mov	eax,320
	mul	ebx
	add	edi,eax
	mov	ecx,256
	xor	al,al
.g1
	or	ecx,ecx
	jz	.ge1
	dec	ecx
	a32 stosb
	inc	al
	jmp	.g1
.ge1
	jmp	.g2
.ge2
	xor	ah,ah
	int	0x16		; await keypress

	mov	ax,0x0003	; switch to 80x25
	int	0x10

	mov	edi,0x000B8000	; write to [es:edi]
	mov	esi,msg3	; display msg3 [ds:esi]
.l3
	a32 lodsb
	or	al,al
	jz	.e3
	a32 stosb
	inc	edi
	jmp	.l3
.e3
	xor	ah,ah
	int	0x16		; await keypress

	leave
	ret

;-----------------------------------------------------------------------------
