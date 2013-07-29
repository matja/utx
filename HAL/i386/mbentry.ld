/*	UTX ld script */

OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(start)
SECTIONS
{
   . = 0x100000;
   
   .text : AT (ADDR(.text) & 0x0fffffff)
   {
     *(.text) *(.text.*)
   } = 0

   . = ALIGN(0x1000);
   .rodata :
   AT (LOADADDR(.text) + (ADDR(.rodata) - ADDR(.text)))
   {
     *(.rodata)
     *(.rodata.*)
   }

   . = ALIGN(0x1000);
   data_start = . ;
   .data : AT (LOADADDR(.text) + (ADDR(.data) - ADDR(.text))) { *(.data) }
   data_end = . ;
 
   . = ALIGN(0x1000); 
   .bss :
   AT (LOADADDR(.text) + (ADDR(.bss) - ADDR(.text)))
   {
     *(.bss)
     *(.bss.*)
     *(COMMON)
   }


}

