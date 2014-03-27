#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "mm.h"

static struct spot *head;
static char * outside;

int mm_init(unsigned long size) {
    /* create a memory pool and initialize data structure */

	char* start = (char*)malloc(sizeof(char)*size);
	outside = start;
	if(start == NULL)
	{
		return -1;
	}
	struct spot *temp = (struct spot *) malloc(sizeof(struct spot));
	temp->ptr = start;
	temp->free = 1;
	temp->size = size;
	head = temp;
	FILE * ftemp2 = fopen("log.txt", "w");
	fclose(ftemp2);
	return 0;
	
}

void printStructure()
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


void printStructureReverse()
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
	struct spot *current = head;

	while(current != NULL) 
	{
		if(current->free == 1) 
		{
			if(current->size >= nbytes) 
			{
				unsigned long diff = current->size - nbytes;
				if(diff > 0) 
				{
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
				return  current->ptr;
			}
		}
	current = current->next;
	}
	int fp = open("log.txt", O_WRONLY | O_APPEND);
	int sout = dup(1);
	fflush(stdout);
        close(1);
        dup(fp);
	printf("Request declined: not enough memory available!\n");
	fflush(stdout);
        close(fp);
	dup2(sout, 1);
	return NULL;
}


int mm_free(char *ptr) {
	/* Check if ptr is valid or not. Only free the valid pointer. Defragmentation. */
	struct spot *current = head;

	while (current != NULL)
	{
//if (current->prev != NULL)
//printf("%d\n", current->prev->free);
		if (current->ptr == ptr && current->free == 0)
		{


			if (current->prev != NULL && current->prev->free == 1)
			{
//printf("bla\n");
				current->prev->size += current->size;
				current->prev->next = current->next;
				current = current->prev;
			}
			else
			{
				current->free = 1;
			}
			if (current->next != NULL && current->next->free == 1)
			{

				current->size += current->next->size;
				current->next = current->next->next;
			}
			return 0;
		}
	current = current->next;
	}
	int fp = open("log.txt", O_WRONLY | O_APPEND);
	int sout = dup(1);
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
	unsigned long nfree = 0;
	struct spot *current = head;
	while (current != NULL)
	{
		if (current->free == 1)
		{
			nfree++;
		}
		current = current->next;
	}
	free(outside);
	*free_num = nfree;
}


/* Extra Credit Part */
int mm_assign(char *ptr, char val) {
	/* Check buffer overflow. If ptr is valid, *ptr=val. */
}

unsigned long mm_check() {
   /* Before calling mm_end, use this function to check memory leaks and return the number of leaking blocks */
}

