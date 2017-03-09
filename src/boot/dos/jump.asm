;
; 
;
; $Id: jump.asm,v 1.1.1.1 1998/02/26 19:01:22 bart Exp $
;

      	.model large
      	.386p

      	.data

      	extrn _gdt_desc : fword
      	extrn _base_mem : dword
	extrn _entry_point  : dword
	extrn _kernel_start : dword
	extrn _kernel_size  : dword

      	.code

long_jump:
      	db 66h
      	db 67h			; 32 bit prefix
      	db 0EAh			; ljmp
entry_point label dword
      	dd 0h			; entry point - filled later
      	dw 8h			; code selector
      	retn     
	


      	public _jump
_jump proc

      	mov edx, _base_mem
      	mov esi, _kernel_start
	mov eax, _entry_point
	mov entry_point, eax
	mov edi, eax
	mov ecx, _kernel_size
	shr ecx, 2

      	cli

      	; load gdt
      	mov ax, SEG _gdt_desc
      	mov ds, ax
      	lgdt _gdt_desc

      	; kick off protection
	;
      	mov   eax, cr0
      	or    eax, 1
      	mov   cr0, eax
      	jmp   $+4
      	nop
      	nop
      	nop
      	nop

      	; ok, we're in protected mode
      	; setup segment registers (who the hell invented these?)
	;
      	mov ax, 10h
      	mov ds, ax
      	mov es, ax
      	mov fs, ax
      	mov gs, ax
      	mov ss, ax

      	jmp   $+4
      	nop
      	nop
      	nop
      	nop
	
      	mov ebx, 0b8000h
      	mov word ptr [ebx+0], 4321h

	; move kernel where due
	; edi, esi, ecx are already set up
	cld
	db 66h
	db 67h
	rep movsd
	
      	mov ebx, 0b8002h
     	mov word ptr [ebx+0], 4321h

	jmp long_jump

      	; the point of no return

_jump endp
      	end
