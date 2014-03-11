#include <stdio.h>
#include "mm.h"

int mm_init(unsigned long size) {
    /* create a memory pool and initialize data structure */
}


char *mm_alloc(unsigned long nbytes) {
    /* allocate nbytes memory space from the pool using first fit algorithm, and return it to the requester */
}


int mm_free(char *ptr) {
	/* Check if ptr is valid or not. Only free the valid pointer. Defragmentation. */
}

void mm_end(unsigned long *alloc_num, unsigned long *free_num) {
	/* Count total allocated blocks. Count total free blocks. Clean up data structure. Free memory pool */
}


/* Extra Credit Part */
int mm_assign(char *ptr, char val) {
	/* Check buffer overflow. If ptr is valid, *ptr=val. */
}

unsigned long mm_check() {
   /* Before calling mm_end, use this function to check memory leaks and return the number of leaking blocks */
}

