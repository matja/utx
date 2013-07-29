
#include "types.h"

struct PageDirectoryPointerTableEntry
{
	uint64_t present:1
		,reserved0:2 // must be 0
		,pwt:1 // page-level write-through
		,pcd:1 // page-level cache disable
		,reserved1:4 // must be 0
		,ignored0:3
		,page_directory_physical_address:52; // pointer to page directory
};

// in PAE mode, CR3 points to this structure
struct PageDirectoryPointerTable
{
	struct PageDirectoryPointerTableEntry pdpte[4];
};

// if PDE.page_size == 0
struct PageDirectoryEntryPAE4k
{
	uint64_t
		present:1
		,read_write:1
		,user_supervisor:1
		,pwt:1
		,pcd:1
		,accessed:1
		,ignored0:1
		,page_size:1
		,ignored1:4
		,page_table_physical_address:51 // pointer to page table
		,execute_disable:1;
};

// if PDE.PS == 1
struct PageDirectoryEntryPAE2M
{
	uint64_t
		present:1
		,read_write:1
		,user_supervisor:1
		,pwt:1
		,pcd:1
		,accessed:1
		,dirty:1
		,page_size:1
		,global:1
		,ignored0:3
		,pat:1
		,reserved0:8 // must be 0
		,page_physical_address:42 // pointer to 2MB page
		,execute_disable:1;
};

struct PageDirectoryPAE
{
	union {
		struct PageDirectoryEntryPAE4k pde4k[512];
		struct PageDirectoryEntryPAE2M pde2M[512];
	};	
};

struct PageDirectoryEntry4k
{
	uint32_t
		present:1
		,read_write:1
		,user_supervisor:1
		,pwt:1
		,pcd:1
		,accessed:1
		,ignored0:1
		,page_size:1
		,ignored1:4
		,page_table_physical_address:20;
};

struct PageDirectoryEntry4M
{
	uint32_t
		present:1
		,read_write:1
		,user_supervisor:1
		,pwt:1
		,pcd:1
		,accessed:1
		,dirty:1
		,page_size:1
		,global:1
		,ignored0:3
		,pat:1
		,page_physical_address_high:4 // for PSE-36
		,reserved:5 // must be 0
		,page_physical_address_low:10	;	
};

struct PageDirectory
{
	union {
		struct PageDirectoryEntry4k pde4k[1024];
		struct PageDirectoryEntry4M pde4M[1024];
	};	
};

struct PageTableEntry4k
{
	uint32_t
		present:1
		,read_write:1
		,user_supervisor:1
		,pwt:1
		,pcd:1
		,accessed:1
		,dirty:1
		,pat:1
		,global:1
		,ignored0:3
		,page_physical_address:20;
};

struct PageTable
{
	struct PageTableEntry4k pte4k[1024];
};




