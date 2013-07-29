CACHE_AS_RAM_BASE equ 0xf2000000
CACHE_AS_RAM_SIZE equ 0x1000
MEMORY_TYPE_WRITEBACK equ 0x06
MTRR_PAIR_VALID 0x800
MTRR_ENABLE 0x800
MTRR_PHYSICAL_BASE_LOW equ (CACHE_AS_RAM_BASE | MEMORY_TYPE_WRITEBACK)
MTRR_PHYSICAL_BASE_HIGH 0x00
MTRR_PHYSICAL_MASH_LOW ((~((CACHE_AS_RAM_SIZE)-0x1)) | MTRR_PAIR_VALID)
MTRR_PHYMASK_HIGH 0xf
IA32_MTRR_PHYSICAL_BASE_REG0 0x200
IA32_MTRR_PHYSICAL_MASK_REG0 0x201
IA32_MTRR_DEF_TYPE_REG 0x2ff
IA32_MISC_ENABLE_REG 0x1a0

; setup MTRR base
mov eax,MTRR_PHYSICAL_BASE_LOW
xor edx,edx
mov ecx,IA32_MTRR_PHYSICAL_BASE_REG0
wrmsr

; setup MTRR mask
mov eax,MTRR_PHYSICAL_MASK_LOW
mov edx,MTRR_PHYSICAL_MASK_HIGH
mov ecx,IA32_MTRR_PHYSICAL_MASK_REG0
wrmsr

; enable MTRR system
mov ecx,IA32_MTRR_DEF_TYPE_REG
rdmsr
or eax,MTRR_ENABLE
wrmsr

; enter normal cache mode
mov eax,cr0
and eax,0x9fffffff
invd
mov cr0,eax

; setup cache tag bits
cld
mov esi,CACHE_AS_RAM_BASE
mov ecx,CACHE_AS_RAM_SIZE/4
rep lodsd

; enter no-fill cache mode
mov eax,cr0
or eax,0x40000000
mov cr0,eax

; initialise CAR
mov edi,CACHE_AS_RAM_BASE
mov ecx,CACHE_AS_RAM_SIZE/4
mov eax,0x12345678
rep stosd

; setup stack
mov esp,CACHE_AS_RAM_BASE+CACHE_AS_RAM_SIZE-4



; check for HT capability
mov eax,1
cpuid
btl edx,28
jnc .skip
bswapl ebx
cmp bh,1
jbe .skip

; setup IPI to tell logical processor(s) to disable cache control
mov ecx,0x250
xor eax,eax
xor edx,edx
wrmsr

SIPI_retry:
mov edi,0xfee00310
mov al,0x01
bswap eax
mov [es:edi],eax
mov edi,0xfee00300
mov eax,0x000006f9
mov [es:edi],eax
mov ecx,0xffff

SIPI_delay:
pause
dec ecx
jnz SPIP_delay
mov edi,0xfee00300
mov eax,[es:edi]
and eax,0x00001000
jnz SIPI_retry

LogicalCPU_SIPI_not_done:
mov ecx,0x250
rdmsr
or eax,eax
jz LogicalCPU_SIPI_not_done

.skip




LogicalCPU_SIPI:
mov eax,cr0
and eax,0x9fffffff
mov cr0,eax
mov ecx,0x250
mov eax,0x01
xor edx,edx
wrmsr
cli
hlt  





