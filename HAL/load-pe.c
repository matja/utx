#include <stdio.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned int leu32;

struct DOSHeader {
        u8 magic[2];
        u8 misc1[58];
        leu32 new_header_offset;
};

struct COFFHeader {
	u8 magic[4];
	u16 machine_architecture;
	u16 sections;
	u32 timestamp;
	u32 symbol_table_pointer;
	u32 symbol_count;
	u16 optional_header_size;
	u16 characteristics;
};

struct PEHeader {
	u16 magic;
	u8 linker_major, linker_minor;
	u32 code_size;
	u32 initialised_data_size;
	u32 uninitialised_data_size;
	u32 entry_point_rva;
	u32 code_base_rva;
	u32 data_base_rva;
};

struct NTHeader {
	u32 image_base;
	u32 section_alignment;
	u32 file_align;
	u16 os_major, os_minor, user_major, user_minor;
	u16 subsystem_major, subsystem_minor;
	u32 reserved;
	u32 image_size, header_size, checksum;
	u16 subsystem, dll_flags;
	u32 stack_reserve_size, stack_commit_size;
	u32 heap_reserve_size, heap_commit_size;
	u32 loader_flags, data_directories;
};

struct PEImage_DataDirectory {
	u32 start, size;	
};

struct PEImage_SectionHeader {
	char name[8];
	u32 virtual_size, virtual_address;
	u32 raw_data_size, raw_data_pointer;
	u32 relocation_pointer, line_number_pointer;
	u16 relocation_count, line_count;
	u32 flags;
};

struct PEImage {
	struct DOSHeader dos_header;
	struct COFFHeader coff_header;
	struct PEHeader pe_header;
	struct NTHeader nt_header;
	struct PEImage_DataDirectory data_directory[16];
	struct PEImage_SectionHeader section_header[16];
};


int main(int argc, char *argv[]) {
	FILE *in;
	
	struct PEImage pe;

	char *filename = NULL;
	u32 i;

	if (argc == 2) {
		filename = argv[1];
	} else {
		fprintf(stderr, "filename required\n");
		return 1;
	}

	in = fopen(filename, "rb");
	if (in == NULL) {
		fprintf(stderr, "unable to open file\n");
		return 1;
	}

	fread(&pe.dos_header, sizeof(pe.dos_header), 1, in);
	if (pe.dos_header.magic[0] != 'M' && pe.dos_header.magic[1] != 'Z') {
		fprintf(stderr, "no MSDOS magic 'MZ' found\n");
		return 1;
	}

	fseek(in, pe.dos_header.new_header_offset, SEEK_SET);
	fread(&pe.coff_header, sizeof(pe.coff_header), 1, in);

	if (pe.coff_header.magic[0] != 'P' && pe.coff_header.magic[1] != 'E') {
		fprintf(stderr, "no PE magic found in COFF header\n");
		return 1;
	}

	printf("COFF machine architecture : 0x%04x\n", pe.coff_header.machine_architecture);
	printf("COFF optional header size : 0x%04x\n", pe.coff_header.optional_header_size);

	fread(&pe.pe_header, sizeof(pe.pe_header), 1, in);
	if (pe.pe_header.magic != 0x010b) {
		fprintf(stderr, "no PE magic found in PE header\n");
		return 1;
	}

	fread(&pe.nt_header, sizeof(pe.nt_header), 1, in);
	printf("NT image base : 0x%08x\n", pe.nt_header.image_base);
	printf("Data directories : %u\n", pe.nt_header.data_directories);

	if (pe.nt_header.data_directories > 16) {
		fprintf(stderr, "excessive number of data_directories (%u)\n",
			pe.nt_header.data_directories);
		return 1;
	}

	fread(&pe.data_directory, sizeof(struct PEImage_DataDirectory),
		pe.nt_header.data_directories, in);

	/* read section headers */
	for (i = 0; i < pe.coff_header.sections; i++) {
		struct PEImage_SectionHeader *section_header = &pe.section_header[i];
		fread(section_header, sizeof(*section_header), 1, in);

		printf("[%-8s] vs=%08x va=%08x rs=%08x rp=%08x\n",
			section_header->name,
			section_header->virtual_size, section_header->virtual_address,
			section_header->raw_data_size, section_header->raw_data_pointer);

		printf("           rp=%08x lp=%08x rc=%04x lc=%04x f=%08x\n",
			section_header->relocation_pointer, section_header->line_number_pointer,
			section_header->relocation_count, section_header->line_count,
			section_header->flags);
	}

	return 0;
}

