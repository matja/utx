#define HEAP_FLAG_ALLOCATED 1
#define HEAP_FLAG_END 2

#include "heap.h"

#include "types.h"

void heap_init(void *start, void *end);
void *heap_allocate(void *heap, size_t size);
void *heap_free(void *heap, void *pointer);

void heap_init(void *start, void *end) {
        *(uint32_t *)start = 0;
	*((uint32_t *)start+1) = (uint8_t *)end - (uint8_t *)start - 12;
	*((uint32_t *)end-1) = HEAP_FLAG_END;
}

void *heap_allocate(void *heap, size_t size) {
	uint8_t *node = (uint8_t *)heap;

	size_t rounded_size = round_up(size, 2);

	while (*(uint32_t *)node != HEAP_FLAG_END) {
		if (*(uint32_t *)node == 0) {
			if (((uint32_t *)node)[1] <= rounded_size) {
				*(uint32_t *)node = HEAP_FLAG_ALLOCATED;
				*(uint32_t *)(node + rounded_size + 8) = 0; /* free marker */
				*(uint32_t *)(node + rounded_size + 12) = *(uint32_t *)(node + 4) - rounded_size - 8;
				*(uint32_t *)(node + 4) -= rounded_size;
				return node + 8;
			}
		}
	}

	return NULL;
}

void *heap_free(void *heap, void *pointer) {
}

void heap_merge_free(void *heap) {
}
