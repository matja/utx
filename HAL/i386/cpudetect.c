#include "cpudetect.h"
#include "types.h"

void cpu_detect(void) {
	uint32_t regs[4] = { 0 };
	char vendor_string[13] = { 0 }; /* 3x 32bit regs, + null */
    char model_string[65] = { 0 }; /* 4 calls of 4x 32bit regs, + null */
    const char *ps = model_string;
    char c = 0, *pd = model_string;
	uint32_t family = 0, model = 0, stepping = 0;

	console_write("detecting CPU ...\n");

	cpuid_i386(0x00, &regs);
	*(uint32_t *)vendor_string = regs[1];
	*((uint32_t *)vendor_string+1) = regs[3];
	*((uint32_t *)vendor_string+2) = regs[2];
	vendor_string[12] = 0;
	console_write("Vendor: ");
	console_write(vendor_string);
	console_write("\n");

	cpuid_i386(0x01, &regs);
	family = (regs[0] >> 8) & 0xf;
	model = (regs[0] >> 4) & 0xf;
	stepping = regs[0] & 0xf;

	regs[4] = 0;
	console_write("Model: ");

	cpuid_i386(0x80000002, &regs);
    *((uint32_t *)model_string+0) = regs[0];
	*((uint32_t *)model_string+1) = regs[1];
	*((uint32_t *)model_string+2) = regs[2];
	*((uint32_t *)model_string+3) = regs[3];

	cpuid_i386(0x80000003, &regs);
    *((uint32_t *)model_string+4) = regs[0];
	*((uint32_t *)model_string+5) = regs[1];
	*((uint32_t *)model_string+6) = regs[2];
	*((uint32_t *)model_string+7) = regs[3];

	cpuid_i386(0x80000004, &regs);
    *((uint32_t *)model_string+8) = regs[0];
	*((uint32_t *)model_string+9) = regs[1];
	*((uint32_t *)model_string+10) = regs[2];
	*((uint32_t *)model_string+11) = regs[3];

	cpuid_i386(0x80000005, &regs);
    *((uint32_t *)model_string+12) = regs[0];
	*((uint32_t *)model_string+13) = regs[1];
	*((uint32_t *)model_string+14) = regs[2];
	*((uint32_t *)model_string+15) = regs[3];

    while (*ps && ps < model_string + sizeof(model_string)) {
        c = *pd++ = *ps++;
        if (c == ' ') {
            while (*ps == ' ') { ps++; }
        }
    }
    *pd = 0;

	console_write(model_string);
	console_write("\n");
}

