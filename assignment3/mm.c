#include <stdio.h>
#include "mm.h"

int mm_init(unsigned long size) {
    /* create a memory pool and initialize data structure */

	char* start = (char*)malloc(sizeof(char)*size);
	if(!(start))
	{
		return -1;
	}
	struct spot temp = {NULL, NULL, NULL, NULL, NULL};
	temp.ptr = start;
	temp.free = true;
	temp.size = size;
	head = temp;
	return 0;
	
}


char *mm_alloc(unsigned long nbytes) {
    /* allocate nbytes memory space from the pool using first fit algorithm, and return it to the requester */
}


int mm_free(char *ptr) {
	struct spot current = head;
	while (!(current))
	{
		if (current.ptr = ptr)
		{
			if (current.prev != NULL && current.prev.free)
			{
				current.prev.size += current.size;
				current.prev.next = current.next;
				current = current.prev;
			}
			else
			{
				current.free = 1;
			}
			if (current.next != NULL && current.next.free == 1)
			{
				current.size += current.next.size;
				current.next = current.next.next;
			}
			return 0;
		}
	current = current.next;
	}
	printf("Free error: not the right pointer!\n");
	return -1;
	/* Check if ptr is valid or not. Only free the valid pointer. Defragmentation. */
}

void mm_end(unsigned long *alloc_num, unsigned long *free_num) {
	/* Count total allocated blocks. Count total free blocks. Clean up data structure. Free memory pool */
	int nallocated = 0;
	int nfree = 0;
	while (!(current))
}


/* Extra Credit Part */
int mm_assign(char *ptr, char val) {
	/* Check buffer overflow. If ptr is valid, *ptr=val. */
}

unsigned long mm_check() {
   /* Before calling mm_end, use this function to check memory leaks and return the number of leaking blocks */
}

