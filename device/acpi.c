
#include "types.h"

struct BIOSDataArea {
	uint16_t com1_base;
	uint16_t com2_base;
	uint16_t com3_base;
	uint16_t com4_base;
	uint16_t lpt1_base;
	uint16_t lpt2_base;
	uint16_t lpt3_base;
	union {
		uint16_t lpt4_base;
		uint16_t ebda_segment;
	};
	uint16_t equipment_word;
	uint8_t interrupt_flag;
	uint16_t memory_size_kb;
	union {
		uint16_t xt_adapter_memory_size;
		uint16_t at_error_codes;
	};
	uint8_t keyboard_shift_flags_1;
	uint8_t keyboard_shift_flags_2;
	uint8_t alt_numpad_work_area;
	uint16_t keyboard_buffer_next;
	uint16_t keyboard_buffer_last;
	uint8_t keyboard_buffer[32];
	uint8_t fdd_calibration_status;
	uint8_t fdd_motor_status;
	uint8_t fdd_motor_timeout;
	uint8_t fdd_status;
};

struct ExtendedBIOSDataArea {
	uint16_t size;
	uint8_t reserved[20];
	uint32_t device_driver_far_call_pointer;
	uint8_t pointing_device_flag[2];
	uint8_t reserved2[8];
};

struct RSDP10 {
	char signature[8];
	uint8_t checksum;
	char oemid[6];
	uint8_t revision;
	uint32_t rsdt_address;
};

struct RSDP20 {
	char signature[8];
	uint8_t checksum;
	char oemid[6];
	uint8_t revision;
	uint32_t rsdt_address;
	uint32_t length;
	uint64_t xsdt_address;
	uint8_t extended_checksum;
	uint8_t reserved[3];
};

enum GenericAddressStructure_address_space {
	GAS_ADDRESS_SPACE_SYSTEM_MEMORY        = 0
	,GAS_ADDRESS_SPACE_SYSTEM_IO           = 1
	,GAS_ADDRESS_SPACE_PCI_CONFIG_SPACE          = 2
	,GAS_ADDRESS_SPACE_EMBEDDED_CONTROLLER       = 3
	,GAS_ADDRESS_SPACE_SMBUS                     = 4
	,GAS_ADDRESS_SPACE_FUNCTIONAL_FIXED_HARDWARE = 0x7f
};

enum GenericAddressStructure_access_size {
	GAS_ACCESS_SIZE_UNDEFINED = 0
	,GAS_ACCESS_SIZE_8 = 1
	,GAS_ACCESS_SIZE_16 = 2
	,GAS_ACCESS_SIZE_32 = 3
	,GAS_ACCESS_SIZE_64 = 4
};


struct GenericAddressStructure
{
	uint8_t address_space;
	uint8_t bit_width;
	uint8_t bit_offset;
	uint8_t access_size;
	uint64_t address;
};

struct FADT {
	struct ACPISDTHeader header;
	uint32_t firmware_control;
	uint32_t dsdt;

	// ACPI 1.0 - not used
	uint8_t reserved;

	uint8_t preferred_power_management_profile;
	uint16_t sci_interrupt;
	uint32_t smi_command_port;
	uint8_t acpi_enable;
	uint8_t acpi_disable;
	uint8_t s4bios_request;
	uint8_t pstate_control;

	uint32_t pm1a_event_block;
	uint32_t pm1b_event_block;
	uint32_t pm2_control_block;
	uint32_t pm_timer_block;
	uint32_t gpe0_block;
	uint32_t gpe1_block;

	uint8_t pm1_event_length;
	uint8_t pm1_control_length;
	uint8_t pm2_control_length;
	uint8_t pmtimer_length;
	uint8_t gpe0_length;
	uint8_t gpe1_length;
	uint8_t gpe1_base;
	uint8_t cstate_control;
	uint16_t worst_c2_latency;
	uint16_t worst_c3_latency;
	uint16_t flush_size;
	uint16_t flush_stride;
	uint8_t duty_offset;
	uint8_t duty_width;
	uint8_t day_alarm;
	uint8_t month_alarm;
	uint8_t centiry;

	// reserved in ACPI 1.0, used since ACPI 2.0
	uint16_t boot_architecture_flags;

	uint8_t reserved_2;
	uint32_t flags;

	GenericAddressStructure ResetReg;

	uint8_t reset_value;
	uint8_t reserved_3[3];

	uint64_t x_firmware_control;
	uint64_t x_dsdt;

	GenericAddressStructure x_pm1a_event_block;
	GenericAddressStructure x_pm1b_event_block;
	GenericAddressStructure x_pm1a_control_block;
	GenericAddressStructure x_pm1b_control_block;
	GenericAddressStructure x_pm12_control_block;
	GenericAddressStructure x_pm_timer_block;
	GenericAddressStructure x_gpe0_block;
	GenericAddressStructure x_gpe1_block;
};

static uint8_t acpi_checksum(const void *start, const void *end)
{
	const uint8_t *start8 = (const uint8_t *)start;
	const uint8_t *end8 = (const uint8_t *)end;
	uint8_t sum = 0;

	while (start8 != end8) {
		sum += *start8++;
	}

	return sum;
}

static int acpi_check_rsdp(const void *rsdp)
{
	static const int fail = -1;
	const struct RSDP10 *rsdp10 = (const struct RSDP10 *)rsdp;
	const struct RSDP10 *rsdp20 = (const struct RSDP20 *)rsdp;

	if (acpi_checksum(rsdp10, rsdp10+sizeof(*rsdp10))) {
		return fail;
	}

	if (rsdp10->revision == 0) {
		console_printf("acpi: found RSDP version 1.0\n");
		return 0;
	}

	if (acpi_checksum(rsdp20, rsdp20+sizeof(*rsdp20))) {
		return rsdp10->revision;
	}

	if (rsdp20->revision == 1) {
		console_printf("acpi: found RSDP version 2.0\n");
		return rsdp20->revision;
	}

	console_printf(
		"acpi: found RSDP version higher than 2.0 (%d) but only 2.0 is supported\n"
		,rsdp20->revision
	);

	return rsdp20->revision;
}

static const char *acpi_find_rsdp_area(const char *start, const char *end)
{
	static const char rsdp_signature[] = "RSD PTR ";
	static const size_t rsdp_align = 16;

	const char *scan_ptr = NULL;

	for (scan_ptr = start; scan_ptr < end; scan_ptr += rsdp_align) {
		if (memcmp(scan_ptr, rsdp_signature, 8) == 0) {
			acpi_check_rsdp(scan_ptr);
		}
	}

	return NULL;
}

static const void *acpi_find_rsdp(void)
{
	static const size_t ebda_size = 1024;
	static const char *bda = (const struct BIOSDataArea *)0x400;
	static const char *ebda_default_start = (const char *)0x07fc00;
	static const char *ebda_default_end   = (const char *)0x0a0000;
	static const char *bios_start         = (const char *)0x0e0000;
	static const char *bios_end           = (const char *)0x100000;

	// get EBDA address from BDA (assuming PS2)
	const char *ebda = (const char *)((uint32_t)bda->ebda_segment << 4);

	// scan provided EBDA
	if (rsdp = acpi_find_rsdp_area(ebda, ebda+ebda_size)) {
		return rsdp;
	}

	// scan default EBDA
	if (rsdp = acpi_find_rsdp_area(ebda_default_start, ebda_default_end)) {
		return rsdp;
	}

	// scan BIOS area
	if (rsdp = acpi_find_rsdp_area(bios_start, bios_end)) {
		return rsdp;
	}

	return NULL;
}

int acpi_init(void)
{
	const void *rsdp = NULL;

	rsdp = acpi_find_rsdp();

	//outb(fadt->smi_command,fadt->acpi_enable);
	return 0;
}

