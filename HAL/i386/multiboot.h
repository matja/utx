#ifndef UTX_INCLUDE_MULTIBOOT_H
#define UTX_INCLUDE_MULTIBOOT_H

#pragma pack(push, 1)
typedef struct MultibootInformationStructure_t {
	uint32_t
		flags
		,mem_lower
		,mem_upper
		,boot_device
		,cmdline
		,mods_count
		,mods_addr
		,syms0
		,syms1
		,syms2
		,syms3
		,mmap_length
		,mmap_addr
		,drives_length
		,drives_addr
		,config_table
		,boot_loader_name
		,apm_table
		,vbe_control_info
		,vbe_mode_info
		,vbe_mode
		,vbe_interface_seg
		,vbe_interface_off
		,vbe_interface_len;
} MultibootInformationStructure;

typedef struct MultibootModule_t {
	uint32_t
		mod_start
		,mod_end
		,cmdline
		,reserved;
} MultibootModule;

typedef struct MultibootMemoryMap_t {
	uint32_t size;
	uint64_t base_addr;
	uint64_t length;
	uint32_t type;
} MultibootMemoryMap;
#pragma pack(pop)

extern MultibootInformationStructure *g_mis;

#endif

