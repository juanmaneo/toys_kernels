;-----------------------------------------------------------------------------
%include "boot.asm"
;-----------------------------------------------------------------------------

mainEntryPoint:

	xor	ah,ah
	int	0x16		; await keypress

	mov	ax,0x0003	; switch to 80x25
	int	0x10

        mov     esi,testMsg	; read testMsg
        mov     edi,0x00200000  ; write at 2MB
.l1
        a32 lodsb
        or      al,al
        jz      .e1
        a32 stosb
        jmp     .l1
.e1

        mov     esi,0x00200000	; read at 2MB
        mov     edi,0x000B8000	; write to screen
.l2
        a32 lodsb
        or      al,al
        jz      .e2
        a32 stosb
        inc     edi
        jmp     .l2
.e2

	xor	ah,ah
	int	0x16		; await keypress

	call	simpleTest
	ret

testMsg	db	"Above 1MB !",0x00

%include "test.asm"
; %include "video.asm"

;-----------------------------------------------------------------------------
BIN_SIZE equ $-$$-512
;-----------------------------------------------------------------------------
%if BIN_SIZE % 512			; to be read, the last used sector
	times 512-(BIN_SIZE % 512) db 0	; must be filled up to 512 bytes
%endif
;-----------------------------------------------------------------------------
