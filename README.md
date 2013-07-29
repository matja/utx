Simple multiboot-compliant kernel I was working on to develop my knowledge of low-level x86 hardware.  Build with `make` in the base directory, which outputs a bootable .iso image which can be started via `qemu-system-x86_64 -cdrom utx.iso -m 16`.

I've implemented a multi-boot header, so multi-boot compliant bootloaders such as GRUB can start the kernel directly.  Simple PIC, PIT, exception, GDT setup is done, so that hardware IRQ's are handled.  I've started on PCI bus enumeration to detect hardware but the output of that is not used yet.

