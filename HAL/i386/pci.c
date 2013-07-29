#include "pci.h"
#include "types.h"
#include "console.h"


static const unsigned PCI_CONFIG_ADDRESS = 0xcf8;
static const unsigned PCI_CONFIG_DATA    = 0xcfc;

void pci_setup_atl1e(uint8_t bus, uint8_t slot, uint8_t func, uint32_t *config_words);


uint32_t pci_address(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
	return (1U << 31)
    	| ((((uint32_t)bus) & 0xff) << 16)
    	| ((((uint32_t)slot) & 0x1f) << 11)
    	| ((((uint32_t)func) & 0x7) << 8)
    	| ((((uint32_t)offset) & 0x3f) << 2);
}

uint32_t pci_config_read_32(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
	out32(PCI_CONFIG_ADDRESS, pci_address(bus, slot, func, offset));
    return in32(PCI_CONFIG_DATA);
}

void pci_config_write_32(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t data)
{
	out32(PCI_CONFIG_ADDRESS, pci_address(bus, slot, func, offset));
	out32(PCI_CONFIG_DATA, data);
}

void pci_config_write_16(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint16_t data)
{
	out32(PCI_CONFIG_ADDRESS, pci_address(bus, slot, func, offset));
	out8(PCI_CONFIG_DATA + (offset & 0x2), data);
}

void pci_config_write_8(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint8_t data)
{
	out32(PCI_CONFIG_ADDRESS, pci_address(bus, slot, func, offset));
	out8(PCI_CONFIG_DATA + (offset & 0x3), data);
}

void pci_dump_device(uint8_t bus, uint8_t slot, uint8_t func)
{
	int result = 0;
	uint16_t vendor, device;
	unsigned config_class, i;
	uint32_t config_words[16];

	for (i=0; i<16; i++) {
		config_words[i] = pci_config_read_32(bus, slot, 0, i);
	}

	vendor = config_words[0] & 0xffff;
	device = (config_words[0] >> 16) & 0xffff;
	config_class = config_words[1] & 0xff;

	console_printf("pci: endpoint: %02x:%02x.%u %04x:%04x class=%u\n"
		,(unsigned)bus
		,(unsigned)slot
		,(unsigned)func
		,(unsigned)vendor
		,(unsigned)device
		,(unsigned)config_class
	);

	// dump base address registers
	for (i=4; i<10; i++) {
		uint32_t bar = config_words[i];
		if (!bar) {
			continue;
		}
		if (bar & 1) {
			// io bar
			uint32_t address = bar & 0xfffffff0;
			unsigned prefetchable = bar & 0x8;
			console_printf("pci: IO  @ %08x (%s)\n"
				,address
				,prefetchable ? "prefetchable" : "non-prefetchable"
			);
		} else {
			// memory bar
			uint32_t address = bar & 0xfffffffc;
			console_printf("pci: mem @ %08x\n", address);
		}
	}


	if (vendor == 0x1969 && device == 0x1026) {
		console_printf("pci: supported device [Atheros L1E Ethernet controller]\n");
		pci_setup_atl1e(bus, slot, func, config_words);
	}

	return;
}

void pci_scan_all(void)
{
	uint32_t bus, slot, func;

	console_printf("pci: scan started\n");
	for (bus = 0; bus < 256; bus++)	{
		for (slot = 0; slot < 32; slot++) {
			for (func = 0; func < 8; func++) {
				uint32_t config_class;

				config_class = pci_config_read_32(bus, slot, func, 2);
				if (config_class == 0xffffffff) {
					if (func == 0) {
						goto next_slot;
					} else {
						continue;
					}
				}

				pci_dump_device(bus, slot, func);
			}
next_slot:
			0;
		}
	}
	console_printf("pci: scan finished\n");
}

int pci_init(void)
{
	pci_scan_all();
	return 0;
}


void pci_setup_atl1e(uint8_t bus, uint8_t slot, uint8_t func, uint32_t *config_words)
{
	unsigned i, data;

	pci_config_read_32(bus, slot, func, 15);

	for (i=0; i < 16; i++) {

		console_printf("atl1e: config: %u = %08x\n"
			,(unsigned)i
			,(unsigned)config_words[i]
		);
	}
}

