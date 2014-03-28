#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "mm.h"

static struct spot *head;			//this will be the first element in our doubly linked list of blocks
static char * outside;

int mm_init(unsigned long size) {
    /* create a memory pool and initialize data structure */

	char* start = (char*)malloc(sizeof(char)*size);		//initialize a pointer for the beginning of the pool
	outside = start;
	if(start == NULL)					//error checking
	{
		return -1;
	}
	struct spot *temp = (struct spot *) malloc(sizeof(struct spot));	//temporary struct for setting values for the static head
	temp->ptr = start;
	temp->free = 1;
	temp->size = size;
	head = temp;								//set head
	FILE * ftemp2 = fopen("out.log", "w");
	fclose(ftemp2);
	return 0;
	
}

void printStructure()			//function used for testing
{
	struct spot *current = head;
printf("\n\n");
	while(current != NULL) 
	{
//		printf("%d\n", current->size);
		printf("%d\n", current->free);
current = current->next;
	}
	
}


void printStructureReverse()		//function used for testing
{
	struct spot *current = head;
	printf("\n\n");
	while(current->next != NULL) 
	{
		current = current->next;
	}
	while (current != NULL)
	{
		printf("%d\n", current->free);
		current = current->prev;
	}
	
}




char *mm_alloc(unsigned long nbytes) {
    /* allocate nbytes memory space from the pool using first fit algorithm, and return it to the requester */
	struct spot *current = head;		//start from head

	while(current != NULL) 			//exit this loop if the end of the memory pool is reached
	{
		if(current->free == 1) 		//if block being looked at is free
		{
			if(current->size >= nbytes) //check if block being looked at is large enough for request
			{
				unsigned long diff = current->size - nbytes;	//calculate "left over" space after allocation
				if(diff > 0)	//sequence below updates block, allocating part of the current free block and making the 
				{		//rest a free block, maintaining the list of structs
//printf("%d\n", (int) nbytes);
					struct spot *freespace = (struct spot *) malloc(sizeof(struct spot));
					freespace->ptr = current->ptr + nbytes;
					freespace->free = 1;
					freespace->size = diff;
					freespace->next = current->next;
					if (freespace->next != NULL)
						freespace->next->prev = freespace;
					freespace->prev = current;
					current->next = freespace;
					current->size = nbytes;
				}
				current->free = 0;
//printf("%d\n", (int) current->next->free);
				return  current->ptr;	//return the pointer on success
			}
		}
		current = current->next;	//iterate through list of blocks
	}
	int fp = open("out.log", O_WRONLY | O_APPEND);	//if no free block is found, record error in out.log
	int sout = dup(1);
	fflush(stdout);
        close(1);
        dup(fp);
	printf("Request declined: not enough memory available!\n");
	fflush(stdout);
        close(fp);
	dup2(sout, 1);
	return NULL;		//return null on failure
}


int mm_free(char *ptr) {
	/* Check if ptr is valid or not. Only free the valid pointer. Defragmentation. */
	struct spot *current = head;		//start at head

	while (current != NULL)
	{
//if (current->prev != NULL)
//printf("%d\n", current->prev->free);
		if (current->ptr == ptr && current->free == 0)	//if the pointer passed is actualy allocated
		{


			if (current->prev != NULL && current->prev->free == 1)	//coalesces free blocks if necessary
			{
//printf("bla\n");
				current->prev->size += current->size;
				current->prev->next = current->next;
				current = current->prev;
			}
			else
			{
				current->free = 1;				//set block pointed to as free
			}
			if (current->next != NULL && current->next->free == 1)	//further handling for coalescing
			{

				current->size += current->next->size;
				current->next = current->next->next;
			}
			return 0;
		}
		current = current->next;
	}
	int fp = open("out.log", O_WRONLY | O_APPEND);	//if we go all the way through the doubly linked list, write appropriate
	int sout = dup(1);				//error to out.log
	fflush(stdout);
        close(1);
        dup(fp);
	printf("Free error: not the right pointer!\n");
	fflush(stdout);
        close(fp);
	dup2(sout, 1);
	return -1;

}

void mm_end(unsigned long *free_num) {
	/* Count total allocated blocks. Count total free blocks. Clean up data structure. Free memory pool */
	unsigned long nfree = 0;	//initialize counter of free blocks to 0
	struct spot *current = head;	//start at head
	while (current != NULL)		//iterate through list of blocks, incrementing counter for every free block
	{
		if (current->free == 1)
		{
			nfree++;
		}
		current = current->next;
	}
	free(outside);
	*free_num = nfree;		//update passed parameter to reflect the number of free blocks
}


/* Extra Credit Part */
int mm_assign(char *ptr, char val) {

	struct spot *current = head;
	while (current != NULL)		//iterate through list of blocks
	{
		if (current->free == 0)	//check for every block that is allocated
		{
			if (ptr >= current->ptr && ptr < current->ptr + current->size)	//if the pointer is inside of an allocated block
			{								//assign the value
				*ptr = val;
				return 0;
			}
		}
		current = current->next;
	}
	int fp = open("out.log", O_WRONLY | O_APPEND);		//if the pointer is not found inside of any allocated block
	int sout = dup(1);					//return -1 and write appropriate error message to out.log
	fflush(stdout);
        close(1);
        dup(fp);
	printf("Buffer Overflow: Try to access illegal memory space.\n");
	fflush(stdout);
        close(fp);
	dup2(sout, 1);
	return -1;
	
}

unsigned long mm_check() {

	unsigned long n_alloc = 0;	//we check for memory leaks by checking the number of allocated blocks that
	struct spot *current = head;	//have not been freed at this stage
	while (current != NULL)		//iterate through blocks
	{
		if (current->free == 0)
		{
			n_alloc++;	//increment counter for every unfreed block at this point
		}
		current = current->next;
	}
return n_alloc;			//return counter

}

