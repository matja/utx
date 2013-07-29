#ifndef UTX_INCLUDE_HAL_PCI_H
#define UTX_INCLUDE_HAL_PCI_H

#include "types.h"

struct PCIDevice
{
	uint8_t bus, slot, func;
};

int pci_init(void);

#endif

