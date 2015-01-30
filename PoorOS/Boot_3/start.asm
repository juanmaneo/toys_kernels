;-----------------------------------------------------------------------------

bits 32 ; boot-loader has switched to 32 bit protected mode

extern mainCode
extern interruptHandler

global __start:function
global __memBegin:data
global __memEnd:data
global __currentStack:data

;-----------------------------------------------------------------------------
section .data
;-----------------------------------------------------------------------------

__memBegin:
	dd	(1024*1024)

__memEnd:
	dd	(MEM_SIZE*1024*1024)

__currentStack:
	dd	0x00000000

%define IDT 0x00000000
%define IDT_LEN ((32+8+8)*8)

idtptr:
	dw	0x0000
	dd	0x00000000

;-----------------------------------------------------------------------------
section .text
;-----------------------------------------------------------------------------
__start:
	cli			; disable interrupts during initialization
	call	setupA20
	call	setupPIC
	call	setupIDT
	finit
	sti			; allow interrupts during main code
	call	mainCode
	cli			; disable interrupts finalization
	call	resetIDT
	sti			; allow interrupts during idle wait
.forever
	hlt			; execution stops here
	jmp	.forever

;-----------------------------------------------------------------------------
setupA20:
.a1
	in	al,0x64		; wait for command-ready
	bt	ax,1
	jc	.a1
	mov	al,0xD0		; send read command
	out	0x64,al
.a2
	in	al,0x64		; wait for read-ready
	bt	ax,0
	jnc	.a2
	in	al,0x60		; read status
	mov	bl,al
.a3
	in	al,0x64		; wait for command-ready
	bt	ax,1
	jc	.a3
	mov	al,0xD1		; send write command
	out	0x64,al
.a4
	in	al,0x64		; wait for write-ready
	bt	ax,1
	jc	.a4
	mov	al,bl
	or	al,0x02		; enable A20
	out	0x60,al		; write status
.a5
	in	al,0x64		; wait for command-ready
	bt	ax,1
	jc	.a5
	mov	al,0xD0		; send read command
	out	0x64,al
.a6
	in	al,0x64		; wait for read-ready
	bt	ax,0
	jnc	.a6
	in	al,0x60		; read status
	bt	ax,1
	jnc	.a1		; loop if A20 is disabled
	ret

;-----------------------------------------------------------------------------
setupPIC:
	mov	al,0xFF		; mask hardware interrupts
	out	0x21,al		; master PIC
	out	0xA1,al		; slave PIC
	jmp	.pic1
.pic1
	mov	al,0x11		; init ICW1
	out	0x20,al		; master PIC
	out	0xA0,al		; slave PIC
	jmp	.pic2
.pic2
	mov	al,0x20		; init ICW2
	out	0x21,al		; master PIC (irq 0-7 --> int 0x20-0x27)
	mov	al,0x28
	out	0xA1,al		; slave PIC (irq 8-F --> int 0x28-0x2F)
	jmp	.pic3
.pic3
	mov	al,0x04		; init ICW3
	out	0x21,al		; master PIC
	mov	al,0x02
	out	0xA1,al		; slave PIC
	jmp 	.pic4
.pic4
	mov	al,0x01		; init ICW4
	out	0x21,al		; master PIC
	out	0xA1,al		; slave PIC
	jmp	.pic5
.pic5
	mov	al,0x00		; unmask hardware interrupts
	out	0x21,al		; master PIC
	out	0xA1,al		; slave PIC
	jmp	.pic6
.pic6
	ret

;-----------------------------------------------------------------------------
setupIDT:
	cld
%assign i 0x00
%rep 0x30			; interrupt 0x00 to 0x2F
        mov     edi,i
        shl     edi,3
        add     edi,IDT         ; destination address
        mov     eax,_idt %+ i
        stosw   
        mov     ax,cs
        stosw
        mov     ax,0x8E00       
        stosw   
        shr     eax,0x10
        stosw   
%assign i i+1
%endrep
	mov	ax,IDT_LEN	; setup and load IDT informations
	mov	[idtptr],ax
	mov	eax,IDT
	mov	[idtptr+2],eax;
	lidt	[idtptr]
	ret

resetIDT:
	cld
	mov	ecx,IDT_LEN
	shr	ecx,3
.l
	mov	edi,ecx
	sub	edi,1
	shl	edi,3
	add	edi,IDT		; destination address
	mov	eax,_idtNoOp
	stosw
	mov	ax,cs
	stosw
	mov	ax,0x8E00
	stosw
	shr	eax,0x10
	stosw
	loop	.l
	mov	ax,IDT_LEN	; setup and load IDT informations
	mov	[idtptr],ax
	mov	eax,IDT
	mov	[idtptr+2],eax;
	lidt	[idtptr]
	ret

;-----------------------------------------------------------------------------

; !!! Stack save/restoration must exactly conform to System::StackFrame !!!

%macro	enterHandler 0
	pusha
	push	ds
	push	es
	push	fs
	push	gs
	sub	esp,(7*4+8*10)
	fsave	[esp]
	mov	eax,[__currentStack]
	mov	[eax],esp
%endmacro

%macro	leaveHandler 0
	mov	eax,[__currentStack]
	mov	esp,[eax]
	frstor	[esp]
	add	esp,(7*4+8*10)
	pop	gs
	pop	fs
	pop	es
	pop	ds
	popa
	iret
%endmacro

%assign i 0x00			; according to Intel, interrupts from
%rep 0x20			; 0x00 to 0x1F are reserved for internal CPU
_idt %+ i:
	enterHandler
	push	dword i
	call	interruptHandler
	add	esp,0x4
	leaveHandler
%assign i i+1
%endrep

%assign i 0x20			; master PIC (irq 0 to irq 7)
%rep 0x08			; mapped to interrupts 0x20-0x27
_idt %+ i:
	enterHandler
	push	dword i
	call	interruptHandler
	add	esp,0x4
	mov	al,0x20
	out	0x20,al		; acknowledge master PIC
	leaveHandler
%assign i i+1
%endrep

%assign i 0x28			; slave PIC (irq 8 to irq 15)
%rep 0x08			; mapped to interrupts 0x28-0x2F
_idt %+ i:
	enterHandler
	push	dword i
	call	interruptHandler
	add	esp,0x4
	mov	al,0x20
	out	0x20,al		; acknowledge master PIC
	out	0xA0,al		; acknowledge slave PIC
	leaveHandler
%assign i i+1
%endrep

_idtNoOp:
	mov	al,0x20
	out	0x20,al		; acknowledge master PIC
	out	0xA0,al		; acknowledge slave PIC
	iret

;-----------------------------------------------------------------------------
