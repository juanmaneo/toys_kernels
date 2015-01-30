;-----------------------------------------------------------------------------

; LOAD_ADDR, BIN_SIZE and GFX_MODE should be defined on the command line

%define FLOPPY_0 0x00
%define FLOPPY_1 0x01
%define HARD_DISK_0 0x80
%define HARD_DISK_1 0x81
%define BOOT_DRIVE FLOPPY_0

%define OLD_ORG 0x7C00
%define NEW_ORG (LOAD_ADDR-512)

            ; the boot-loader, which is placed by BIOS at 0000:OLD_ORG,
org NEW_ORG ; will be moved down to 0000:NEW_ORG to make room above

bits 16 ; startup is done in real mode

%define GDT_LEN (GDT.end-GDT)
%define GDT_CS  (GDT.cs-GDT)
%define GDT_DS  (GDT.ds-GDT)
%define GDT_SS  (GDT.ss-GDT)

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

	mov	si,OLD_ORG	; source [ds:si]
	mov	di,NEW_ORG	; destination [es:di]
	mov	cx,256		; 512 bytes
.l
	lodsw
	stosw
	loop	.l

	push	(NEW_ORG>>4)	; load new code segment
	push	(.here-NEW_ORG)	; and branch to moved code
	retf
.here
	push	(LOAD_ADDR>>4)	; baseSegment
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

%if GFX_MODE != 0
	mov	ax,0x0013	; use (320x200x256) graphic mode
	int	0x10
%else
	mov	al,0x03		; use (80x25x16) text mode
	mov	ah,0x00
	int	0x10
	mov	al,0x00		; use first text page
	mov	ah,0x05
	int	0x10
	mov	ch,0x20		; hide cursor
	mov	cl,0x20
	mov	ah,0x01
	int	0x10
%endif

	cli			; avoid interrupts

	mov	ax,GDT_LEN	; init and load GDT
	mov	[GDT.null],ax
	xor	eax,eax
	mov	ax,ds
	shl	eax,4
	add	eax,GDT
	mov	[GDT.null+2],eax
	lgdt	[GDT]

	mov	eax,cr0		; switch to protected mode
	or	al,0x01
	mov	cr0,eax

	mov	ax,GDT_DS	; load data segment descriptors
	mov	ds,ax
	mov	es,ax
	mov	fs,ax
	mov	gs,ax

	mov	ax,GDT_SS	; load stack segment descriptor
	mov	ss,ax
	mov	esp,0x0009FFF0	; create stack under segment 0xA000

	push	GDT_CS		; load code segment descriptor
	push	LOAD_ADDR	; and branch to loaded code
	retf

;-----------------------------------------------------------------------------

GDT:
.null	db	0		; limit 7:0
	db	0		; limit 15:8
	db	0		; base 7:0
	db	0		; base 15:8
	db	0		; base 23:16
	db	0		; access rights
	db	0		; granularity, op-size, limit 19:16
	db	0		; base 31:24
.cs	db	0xFF		; limit 0xFFFFF (1 meg? 4 gig?)
	db	0xFF		;
	db	0		; base 0
	db	0		;
	db	0		;
	db	0x9A		; (present,ring 0,code,?,readable)
	db	0xCF		; granularity=4k,32-bit
	db	0		;
.ds	db	0xFF		; limit 0xFFFFF (1 meg? 4 gig?)
	db	0xFF		;
	db	0		; base 0
	db	0		;
	db	0		;
	db	0x92		; (present,ring 0,data,up,writable)
	db	0xCF		; granularity=4k,32-bit
	db	0		;
.ss	db	0		; limit 0 (down)
	db	0		;
	db	0		; base 0
	db	0		;
	db	0		;
	db	0x96		; (present,ring 0,data,down,writable)
	db	0xC0		; granularity=4k,32-bit
	db	0		;
.end

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
