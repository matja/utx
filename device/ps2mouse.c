
#include "acpi.h"

/*
Intel 8042-compatible PS2 controller.

channel 0 : keyboard -> IRQ1 (PIC1)
channel 1 : mouse -> IRQ12 (PIC2)
channel 2 : speaker enable

IO ports :
0x60 : R/W : data
0x64 : R : status
0x64 : W : command

TODO:
check bit 1 (the "8042" flag) in the "IA PC Boot Architecture Flags" field at offset 109 in the Fixed ACPI Description Table (FADT). If this bit is clear then there is no PS/2 Controller to configure. Otherwise, if the bit is set or the system doesn't support ACPI (no ACPI tables and no FADT) then there is a PS/2 Controller.

*/

#define ACPI_FADT_IA_PC_BOOT_ARCH_FLAGS 109

static const char *module_name = "ps2mouse";

void ps2mouse_init(void)
{
	int ps2_controller_present = 1;

	if (acpi_enabled()) {
		uint8_t flags =	acpi_fadt_read(ACPI_FADT_IA_PC_BOOT_ARCH_FLAGS);
		ps2_controller_present = flags & 1;
		console_printf("%s: ACPI reports PS2 controller %s\n"
			,ps2_controller_present ? "present" : "not present"
		);
	}

	if (!ps2_controller_present) {
		return;
	}

	mouse_wait(1);
	out8(MOUSE_STATUS, 0xA8);
	mouse_wait(1);
	out8(MOUSE_STATUS, 0x20);
	mouse_wait(0);
	status = in8(0x60) | 2;
	mouse_wait(1);
	out8(MOUSE_STATUS, 0x60);
	mouse_wait(1);
	out8(MOUSE_PORT, status);
	mouse_write(0xF6);
	mouse_read();
	mouse_write(0xF4);
	mouse_read();
	IRQ_RES;
	irq_install_handler(MOUSE_IRQ, mouse_handler);
}


