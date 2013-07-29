bits 32

%include "arch_i386.inc"

global arch_main
global out8
global out16
global out32
global in8
global in16
global in32
global cpuid_i386
global dump_regs
global gdt_flush
global tss_flush
global idt_flush
global irq_enable
global irq_disable
global irq_wait
global irq_handlers
global irq_handler_default
global irq_handler_0
global irq_handler_1
global irq_handler_4
global exception_handler_default
global exception_test
global g_gdt
global g_gdt_end
global g_gdt_ptr
global g_idt
global g_idt_end
global g_idt_ptr
global tss
global tss_end
global tss_ss0
global tss_esp0
global tss_iomap_base

extern main
extern console_write
extern console_printf

section .text

arch_main:
	call main	

	push eax
	push arch_main__format
	call console_printf
	add esp, 8
	jmp hang
	
hang:
	cli
	hlt
	jmp hang
	

gdt_flush:
	push eax
	
	lgdt [g_gdt_ptr]	
	
	mov ax, SELECTOR_KERNEL_DATA
	mov ds, ax
	mov es, ax
	mov ss, ax
	jmp SELECTOR_KERNEL_CODE:gdt_flush__jmp
gdt_flush__jmp:

	pop eax
	ret
	
	
idt_flush:
	lidt [g_idt_ptr]	
	ret	



%macro EXCEPTION_HANDLER 1
global exception_handler_%1
exception_handler_%1:
	push exception_handler__message
	push exception_handler_%1__name
	call console_printf
	add esp, 4
	call dump_regs
	jmp hang
%endmacro

EXCEPTION_HANDLER DE
EXCEPTION_HANDLER DB
EXCEPTION_HANDLER NMI
EXCEPTION_HANDLER BP
EXCEPTION_HANDLER OF
EXCEPTION_HANDLER BR	
EXCEPTION_HANDLER UD
EXCEPTION_HANDLER NM	
EXCEPTION_HANDLER DF
EXCEPTION_HANDLER R0
EXCEPTION_HANDLER TS
EXCEPTION_HANDLER NP
EXCEPTION_HANDLER SS
EXCEPTION_HANDLER GP	
EXCEPTION_HANDLER PF
EXCEPTION_HANDLER R1
EXCEPTION_HANDLER MF
EXCEPTION_HANDLER AC
EXCEPTION_HANDLER MC
EXCEPTION_HANDLER XM

irq_handler_0:
	pushad
	mov eax, [irq_handlers+0*4]
	call eax
	popad
	iretd


irq_handler_1:
	pushad
	call reboot
	mov eax, [irq_handlers+1*4]
	call eax
	popad
	iretd
	
	
irq_handler_4:
	pushad
	mov eax, [irq_handlers+4*4]
	call eax
	popad
	iretd


irq_handler_default:
	push irq_handler_default__message
	call console_write
	add esp, 4
	call dump_regs	
irq_handler_default__hang:
	hlt
	jmp irq_handler_default__hang

	
irq_enable:
	sti
	ret


irq_disable:
	cli
	ret

	
irq_wait:
	hlt
	ret


out8:
	push edx
	mov eax, [esp+12]
	mov edx, [esp+8]
	out dx, al
	pop edx
	ret

out16:
	push edx
	mov eax, [esp+12]
	mov edx, [esp+8]
	out dx, ax
	pop edx
	ret
	
out32:
	push edx
	mov eax, [esp+12]
	mov edx, [esp+8]
	out dx, eax
	pop edx
	ret	
	
in8:
	push edx
	mov edx, [esp+8]
	xor eax,eax
	in al, dx
	pop edx
	ret
	
in16:
	push edx
	mov edx, [esp+8]
	xor eax,eax
	in ax, dx
	pop edx
	ret
	
in32:
	push edx
	mov edx, [esp+8]
	xor eax,eax
	in eax, dx
	pop edx
	ret
	
	
reboot:
	cli
	mov dx, 0x64
reboot__loop:
	in al, dx
	test al, 2
	jnz reboot__loop
	mov al, 0xfe
	out dx, al
reboot__halt:
	hlt
	jmp reboot__halt


cpuid_i386:
	push ebx
	push edi

	mov eax, [esp+12]
	mov edi, [esp+16]
	cpuid
	mov [edi], eax
	mov [edi+4], ebx
	mov [edi+8], ecx
	mov [edi+12], edx

	pop edi
	pop ebx
	ret

dump_regs__format:
	db "CS=%04x DS=%04x ES=%04x SS=%04x FS=%04x GS=%04x",0x0a
	db "CR0=%08x CR2=%08x CR3=%08x CR4=%08x",0x0a	
	db "EFLAGS=%08x EIP=%08x",0x0a
	db "EDI=%08x ESI=%08x EBP=%08x ESP=%08x",0x0a
	db "EBX=%08x EDX=%08x ECX=%08x EAX=%08x",0x0a	
	db 0

dump_regs:
	pushad
	call dump_regs__next
dump_regs__next:
	pushf
	
	xor eax,eax
	mov eax,cr4
	push eax	
	mov eax,cr3
	push eax	
	mov eax,cr2
	push eax
	mov eax,cr0
	push eax

	mov ax,gs
	push eax
	mov ax,fs
	push eax	
	mov ax,ss
	push eax	
	mov ax,es
	push eax
	mov ax,ds
	push eax
	mov ax,cs
	push eax
	
	push dump_regs__format
	call console_printf

	add esp,84	
	ret
	


global tss_init
tss_init:

	ret


tss_flush:
	push eax
	mov ax, 0x08 | 3
	ltr ax
	pop eax
	ret

global __udivdi3
__udivdi3:
	push edx
	mov eax,[esp+12]
	div dword [esp+8]
	pop edx
	ret

section .data


; global descriptor table
align 16
g_gdt:
%rep 6
	dw 0,0,0,0
%endrep
g_gdt_end:

align 16
g_gdt_ptr:
	dw g_gdt_end - g_gdt - 1
	dd g_gdt

; interrupt descriptor table
align 16
g_idt:
%rep 256
	dw 0 ; offset low
	dw 0 ; selector
	dw 0 ; flags
	dw 0 ; offset high
%endrep
g_idt_end:

align 16
g_idt_ptr:
	dw g_idt_end - g_idt - 1
	dd g_idt

; align 16
tss:
tss_prev_tss: dd 0
tss_esp0: dd 0
tss_ss0: dd 0
tss_esp1: dd 0
tss_ss1: dd 0
tss_esp2: dd 0
tss_ss2: dd 0
tss_cr3: dd 0
tss_eip: dd 0
tss_eflags: dd 0
tss_eax: dd 0
tss_ecx: dd 0
tss_edx: dd 0
tss_ebx: dd 0
tss_esp: dd 0
tss_ebp: dd 0
tss_esi: dd 0
tss_edi: dd 0
tss_es: dd 0
tss_cs: dd 0
tss_ss: dd 0
tss_ds: dd 0
tss_fs: dd 0
tss_gs: dd 0
tss_ldt: dd 0
tss_trap: dw 0
tss_iomap_base: dw 0
tss_end:

arch_main__format: db "main() exited (%u)",0x0a,0

exception_handler__message:
	db "%s: ec=%08x EIP=%08x CS=%04x EFLAGS=%08x", 0x0a, 0

exception_handler_DE__name: db "DIVIDE ERROR",0
exception_handler_DB__name: db "DEBUG EXCEPTION",0
exception_handler_NMI__name: db "NMI",0
exception_handler_BP__name: db "BREAKPOINT",0
exception_handler_OF__name: db "OVERFLOW",0
exception_handler_BR__name: db "BOUND RANGE",0
exception_handler_UD__name: db "UNDEFINED OPCODE",0
exception_handler_NM__name: db "NO MATH COPROCESSOR",0
exception_handler_DF__name: db "DOUBLE FAULT",0
exception_handler_R0__name: db "COPROCESSOR SEGMENT OVERRUN",0
exception_handler_TS__name: db "INVALID TSS",0
exception_handler_NP__name: db "SEGMENT NOT PRESENT",0
exception_handler_SS__name: db "STACK SEGMENT FAULT",0
exception_handler_GP__name: db "GENERAL PROTECTION FAULT",0
exception_handler_PF__name: db "PAGE FAULT",0
exception_handler_R1__name: db "RESERVED 15",0
exception_handler_MF__name: db "MATH FAULT",0
exception_handler_AC__name: db "ALIGNMENT CHECK",0
exception_handler_MC__name: db "MACHINE CHECK",0
exception_handler_XM__name: db "SIMD EXCEPTION",0
	
irq_handler_default__message: db "UNHANDLED IRQ", 0x0a, 0
irq_handler_0__message: db "0", 0
irq_handler_1__message: db "1", 0

irq_handlers:
%rep 32
	dd 0
%endrep

