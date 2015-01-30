;-----------------------------------------------------------------------------

%define FLOPPY_0 0x00
%define FLOPPY_1 0x01
%define HARD_DISK_0 0x80
%define HARD_DISK_1 0x81
%define BOOT_DRIVE FLOPPY_0

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

	push	((ORG+512)>>4)	; baseSegment
	mov	eax,BIN_SIZE
	mov	ebx,eax
	shr	eax,9
	test	ebx,0x000001FF	
	jz	.noinc
	inc	eax
.noinc
	push	ax		; nbSectors
	call	loadBinary
	add	sp,0x4

	call	mainEntryPoint

	int	0x19		; warm boot computer


;-----------------------------------------------------------------------------
; void
; loadBinary(word nbSectors,
;            word baseSegment)
;-----------------------------------------------------------------------------
loadBinary:
	push	bp
	mov	bp,sp
	call	initDrive
	mov	cx,[bp+4]	; nbSectors
	mov	ax,(512>>4)
	mul	cx
	add	ax,[bp+6]	; baseSegment
.l
	sub	ax,(512>>4)
	push	ax
	push	cx
	call	loadDriveSector
	pop	cx
	pop	ax
	loop	.l
	leave
	ret

;-----------------------------------------------------------------------------
; void
; initDrive(void)
;-----------------------------------------------------------------------------
driveGeometry:
.nbhead	dw	0
.nbcyl	dw	0
.nbsect	dw	0

initDrive:
	push	bp
	mov	bp,sp
	xor	ah,ah		; reset drive
	mov	dl,BOOT_DRIVE
	int	0x13
	mov	ah,0x08		; read drive geometry
	mov	dl,BOOT_DRIVE
	int	0x13
	xor	ax,ax
	mov	al,dh
	inc	ax
	mov	[driveGeometry.nbhead],ax
	xor	ax,ax
	mov	al,cl
	shl	ax,2
	mov	al,ch
	inc	ax
	mov	[driveGeometry.nbcyl],ax
	xor	ax,ax
	mov	al,cl
	and	ax,0x003F
	mov	[driveGeometry.nbsect],ax
	leave
	ret

;-----------------------------------------------------------------------------
; void
; loadDriveSector(word sector,
;                 word segment)
;-----------------------------------------------------------------------------
loadDriveSector:
	push	bp
	mov	bp,sp
	xor	dx,dx
	mov	ax,[bp+4]	; sector
	div	word [driveGeometry.nbsect]
	inc	dx
	mov	cx,dx		; S=(LBA%NBS)+1
	xor	dx,dx
	div	word [driveGeometry.nbhead]
	mov	dh,dl		; H=(LBA/NBS)%NBH
	mov	ch,al		; C=(LBA/NBS)/NBH
	mov	dl,BOOT_DRIVE
	mov	es,[bp+6]	; segment
	xor	bx,bx
	mov	al,1		; 1 sector
	mov	ah,0x02
	int	0x13
	leave
	ret

;-----------------------------------------------------------------------------
	times 510-($-$$) db 0	; the boot sector (512 bytes) must end with
	db 0x55, 0xAA		; 55 AA to be loaded by BIOS
;-----------------------------------------------------------------------------
