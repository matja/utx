
#include "types.h"
#include "multiboot.h"
#include "console.h"

void multiboot_print(void)
{
	uint32_t flags = 0;

	console_printf("multiboot: structure at %p\n", g_mis);

	if (!g_mis) {
		return;
	}

	flags = g_mis->flags;

	if (flags & 0x1) {
		console_printf(" memory: lower %ukB upper %ukB\n",
			(unsigned)g_mis->mem_lower,
			(unsigned)g_mis->mem_upper
		);
	}

	if (flags & 0x4) {
		const char *cmdline = *(const char **)&g_mis->cmdline;
		if (cmdline) {
			console_printf(" command line: [%s]\n", cmdline);
		}
	}

	if (flags & 0x8) {
		const MultibootModule *mod = (const MultibootModule *)g_mis->mods_addr;
		unsigned i = 0;

		console_printf(" modules: count:%u address:%p\n"
			,(unsigned)g_mis->mods_count
			,(void *)g_mis->mods_addr
		);

		i = 0;

		while (i < g_mis->mods_count) {
			console_printf("  start:%p end:%x cmdline:[%s]\n"
				,(unsigned)mod->mod_start
				,(unsigned)mod->mod_end
				,(char *)mod->cmdline
			);

			i++;
			mod++;
		}
	}

	if (flags & 0x10) {
		console_printf(" symbol table: \n");
	}

	if (flags & 0x40) {
		const MultibootMemoryMap *map_start, *map;
		const char *type_string;

		console_printf(" memory map: %u bytes at 0x%p :\n",
			(unsigned)g_mis->mmap_length,
			*(const void **)&g_mis->mmap_addr
		);

		if (g_mis->mmap_addr) {
			const MultibootMemoryMap *map = (const MultibootMemoryMap *)g_mis->mmap_addr;
			unsigned bytes_left = g_mis->mmap_length;

			while (bytes_left >= sizeof(*map)) {
				if (map->type == 1) {
					type_string = "available";
				} else if (map->type == 2) {
					type_string = "reserved";
				} else if (map->type == 3) {
					type_string = "ACPI reclaim";
				} else if (map->type == 4) {
					type_string = "ACPI NVS";
				} else if (map->type == 5) {
					type_string = "bad";
				} else {
					type_string = "reserved";
				}

				console_printf("  %016llx : %016llx : %08x (%s)\n",
					(unsigned long long)map->base_addr,
					(unsigned long long)map->length,
					(unsigned)map->type,
					type_string
				);
				map++;
				bytes_left -= 24;
			}
		}
	}

	if (flags & 0x100) {
		console_printf(" BIOS configuration:\n");
	}

	if (flags & 0x200) {
		const char *boot_loader_name = *(const char **)&g_mis->boot_loader_name;
		if (boot_loader_name) {
			console_printf(" bootloader name:[%s]\n", boot_loader_name);
		}
	}

	if (flags & 0x800) {
		console_printf(" graphics table:\n");					
	}
}

