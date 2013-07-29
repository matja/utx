
.PHONY : all clean fonts HAL livecd

LIVECD = output/livecd.iso

all : livecd

clean :
	rm -r output/*

livecd : $(LIVECD)

$(LIVECD) : fonts HAL
	mkisofs \
		-o $@ -m .svn --joliet-long -r -graft-points -input-charset=utf8 \
		-boot-info-table -boot-load-size 4 -no-emul-boot -c boot/boot.cat \
		-b boot/grub/stage2_eltorito \
		boot/grub=3rdparty/grub \
		utx/hal=output/i386/HAL

fonts :
	$(MAKE) -C fonts

HAL :
	$(MAKE) -C HAL
	cp output/i386/HAL /tftproot/utx

qemu : livecd
	qemu-system-x86_64 -m 128 -cdrom $(LIVECD) -vga std

bochs : livecd
	bochs -qf cd.bochs

