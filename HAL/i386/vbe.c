
#include "console.h"
#include "types.h"

struct PMInfoBlock {
	char signarture[4];
	uint16_t entry;
	uint16_t pm_init;
	uint16_t bios_data_sel;
	uint16_t sel_a000;
	uint16_t sel_b000;
	uint16_t sel_b800;
	uint16_t code_seg_sel;
	uint8_t in_pmode;
	uint8_t checksum;
};

void vbe3_scan(void) {
    /*
    search for VBE/Core 3.0 Protected Mode Interface (PMI)
    */

	const uint8_t *start = (const uint8_t *)0xc0000; // start from first ROM
	unsigned int found = 0;

	console_write("scanning VGA BIOS for VBE3 support ... ");

	while (start < (const uint8_t *)0xffff8) {
		if (start[0] == 'P' && start[1] == 'M' &&
			start[2] == 'I' && start[3] == 'D') {
			console_write("possible at ");
			console_write_address(start);
			console_write(" ");
			found++;
		}
		start++;
	}

	if (found == 0) {
		console_write("not found\n");
	}
	if (found == 1) {
		console_write("ok - 1 found\n");
	}
	if (found > 1) {
		console_write("error - multiple, ignoring\n");
	}
}

