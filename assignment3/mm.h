#ifndef __MM_H
#define __MM_H

/* You are free to declare any data type here */

int mm_init(unsigned long size);
char *mm_alloc(unsigned long nbytes);
int mm_free(char *ptr);
void mm_end(unsigned long *free_num);
int mm_assign(char *ptr, char val);
unsigned long mm_check();
void printStructure();		//leftover functions used for testing
void printStructureReverse();

struct spot {			//this is the struct we use to represent blocks of memory
	char* ptr;		//pointer to the beginning of the block
	int free;		//0 or 1 value, acting as a boolean saying whether the block is free or allocated
	unsigned long size;	//size of the block
	struct spot *prev;	//pointers to previous and next elements in our doubly linked list of blocks
	struct spot *next;
};







#endif
