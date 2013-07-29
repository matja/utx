
.PHONY : all clean

OUTPUT=../../output/i386
CC=gcc
CC_OPTS=-m32 -Os -fomit-frame-pointer -fno-builtin
AS=nasm
AS_OPTS=-f elf

OBJECTS=\
	$(OUTPUT)/mbentry.o \
	$(OUTPUT)/multiboot.o \
	$(OUTPUT)/arch_i386.o \
	$(OUTPUT)/main.o \
	$(OUTPUT)/stdlib.o \
	$(OUTPUT)/heap.o \
	$(OUTPUT)/cpudetect.o \
	$(OUTPUT)/vbe.o \
	$(OUTPUT)/console.o \
	$(OUTPUT)/pic.o \
	$(OUTPUT)/pit.o \
	$(OUTPUT)/pci.o \
	$(OUTPUT)/vga.o \
	../../fonts/fonts.o

all : $(OUTPUT) $(OUTPUT)/HAL

clean :
	rm $(OUTPUT)/HAL $(OBJECTS)

$(OUTPUT) :
	mkdir -p $(OUTPUT)

$(OUTPUT)/HAL : mbentry.ld Makefile $(OBJECTS)
	ld -z max-page-size=4096 -T mbentry.ld -Map=$(OUTPUT)/HAL.map -o $(OUTPUT)/HAL $(OBJECTS)
	strip -s -R .comment $@

$(OUTPUT)/arch_i386.o : arch_i386.asm
	$(AS) $(AS_OPTS) -o $@ $<

$(OUTPUT)/mbentry.o : mbentry.asm Makefile
	$(AS) $(AS_OPTS) -o $@ $<

$(OUTPUT)/main.o : main.c Makefile
	$(CC) $(CC_OPTS) -c -o $@ $<

$(OUTPUT)/multiboot.o : multiboot.c Makefile
	$(CC) $(CC_OPTS) -c -o $@ $<

$(OUTPUT)/stdlib.o : stdlib.c Makefile
	$(CC) $(CC_OPTS) -c -o $@ $<

$(OUTPUT)/heap.o : heap.c Makefile
	$(CC) $(CC_OPTS) -c -o $@ $<

$(OUTPUT)/console.o : console.c Makefile
	$(CC) $(CC_OPTS) -c -o $@ $<

$(OUTPUT)/vbe.o : vbe.c Makefile
	$(CC) $(CC_OPTS) -c -o $@ $<

$(OUTPUT)/cpudetect.o : cpudetect.c Makefile
	$(CC) $(CC_OPTS) -c -o $@ $<

$(OUTPUT)/pic.o : pic.c Makefile
	$(CC) $(CC_OPTS) -c -o $@ $<

$(OUTPUT)/pit.o : pit.c Makefile
	$(CC) $(CC_OPTS) -c -o $@ $<

$(OUTPUT)/vga.o : vga.c Makefile
	$(CC) $(CC_OPTS) -c -o $@ $<

$(OUTPUT)/pci.o : pci.c Makefile
	$(CC) $(CC_OPTS) -c -o $@ $<

