%include "arch_i386.inc"

global start

global g_mis
global g_gdt
global g_gdt_end
global g_gdt_ptr
global g_idt
global g_idt_end
global g_idt_ptr

section .text

extern arch_main
extern data_end

%define MULTIBOOT_MAGIC	0x1BADB002

%define MULTIBOOT_REQUEST_PAGE_ALIGN	0x1
%define MULTIBOOT_REQUEST_MEMORY_INFO	0x2
%define MULTIBOOT_REQUEST_VIDEO_MODE	0x4
%define MULTIBOOT_REQUEST_AOUT_KLUDGE	0x10000

%define MULTIBOOT_FLAGS	( \
	MULTIBOOT_REQUEST_PAGE_ALIGN \
	| MULTIBOOT_REQUEST_MEMORY_INFO \
	| MULTIBOOT_REQUEST_VIDEO_MODE \
	)
%define MULTIBOOT_CHECKSUM	(0 - MULTIBOOT_FLAGS - MULTIBOOT_MAGIC)
%define STACKSIZE	0x4000

start:
jmp multiboot_entry

align 4
multiboot_header:
	dd MULTIBOOT_MAGIC
	dd MULTIBOOT_FLAGS
	dd MULTIBOOT_CHECKSUM

	dd 0x00000000 ; header_addr
	dd 0x00000000 ; load_addr
	dd 0x00000000 ; load_end_addr
	dd 0x00000000 ; bss_end_addr
	dd 0x00000000 ; entry_addr
	
; graphics request flags
	dd 0
	dd 640
	dd 480
	dd 0         
	

align 16
multiboot_entry:
	cli
	cld
	mov esp, stack+STACKSIZE-4
	mov [g_mis], ebx
	jmp arch_main	

section .data
		
; multiboot specification information structure
align 16
g_mis: dd 0	

section .bss
stack:
	resb STACKSIZE
end:


