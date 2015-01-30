;-----------------------------------------------------------------------------

V80x25 equ 0x000B8000

V320x200 equ 0x000A0000

;-----------------------------------------------------------------------------
; void
; init80x25(void)
;-----------------------------------------------------------------------------
init80x25:
	mov	ax,0x0003
	int	0x10
	ret

;-----------------------------------------------------------------------------
; void
; init320x200(void)
;-----------------------------------------------------------------------------
init320x200:
	mov	ax,0x0013
	int	0x10
	ret

;-----------------------------------------------------------------------------
; void
; pixel(word x,
;       word y,
;       byte color)
;-----------------------------------------------------------------------------
pixel:
	push	bp
	mov	bp,sp
	mov	edi,V320x200
	xor	eax,eax
	mov	ax,[bp+4]	; x
	add	edi,eax
	mov	ax,[bp+6]	; y
	shl	eax,6
	add	edi,eax
	shl	eax,2
	add	edi,eax		; V320x200 + x + y*(64+256)
	mov	ax,[bp+8]	; color
	a32 stosb
	leave
	ret

;-----------------------------------------------------------------------------
; void
; lineH(word x,
;       word y,
;       word length,
;       byte color)
;-----------------------------------------------------------------------------
lineH:
	push	bp
	mov	bp,sp
	mov	edi,V320x200
	xor	eax,eax
	mov	ax,[bp+4]	; x
	add	edi,eax
	mov	ax,[bp+6]	; y
	shl	eax,6
	add	edi,eax
	shl	eax,2
	add	edi,eax		; V320x200 + x + y*(64+256)
	xor	ecx,ecx
	mov	cx,[bp+8]	; length
	mov	ax,[bp+10]	; color
	a32 rep stosb
	leave
	ret

;-----------------------------------------------------------------------------
; void
; lineV(word x,
;       word y,
;       word length,
;       byte color)
;-----------------------------------------------------------------------------
lineV:
	push	bp
	mov	bp,sp
	mov	edi,V320x200
	xor	eax,eax
	mov	ax,[bp+4]	; x
	add	edi,eax
	mov	ax,[bp+6]	; y
	shl	eax,6
	add	edi,eax
	shl	eax,2
	add	edi,eax		; V320x200 + x + y*(64+256)
	mov	cx,[bp+8]	; length
	mov	ax,[bp+10]	; color
.loop
	or	cx,cx
	jz	.end
	a32 stosb
	dec	cx
	add	edi,319
	jmp	.loop
.end
	leave
	ret

;-----------------------------------------------------------------------------

simpleTest:
	push	bp
	mov	bp,sp
	sub	sp,2		; local word

	xor	ax,ax
	mov	ds,ax
	mov	es,ax
	mov	fs,ax
	mov	gs,ax

	call	init320x200

	mov	word [bp-2],199
.loop1
	mov	ax,[bp-2]
	or	ax,ax
	jz	.end1
	push	ax		; color
	inc	ax
	push	ax		; length
	dec	ax
	push	ax		; y
	push	0		; x
	call	lineH
	add	sp,6
	dec	word [bp-2]
	jmp	.loop1
.end1

	mov	word [bp-2],255
.loop2
	mov	ax,[bp-2]
	or	ax,ax
	jz	.end2
	push	ax		; color
	push	ax		; length
	push	0		; y
	push	ax		; x
	call	lineV
	add	sp,6
	dec	word [bp-2]
	jmp	.loop2
.end2

	xor     ah,ah
	int     0x16            ; await keypress

	call	init80x25

	mov	edi,V80x25	; raw output
	mov	esi,backMsg
	mov	ax,0x0700
.loop3
	a32 lodsb
	or	al,al
	jz	.end3
	a32 stosw
	jmp	.loop3
.end3

	xor     ah,ah
	int     0x16            ; await keypress

	leave
	ret

backMsg	db "Back to text mode !",0

;-----------------------------------------------------------------------------
