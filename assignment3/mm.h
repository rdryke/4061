#ifndef __MM_H
#define __MM_H

/* You are free to declare any data type here */

int mm_init(unsigned long size);
char *mm_alloc(unsigned long nbytes);
int mm_free(char *ptr);
void mm_end(unsigned long *alloc_num, unsigned long *free_num);
int mm_assign(char *ptr, char val);
unsigned long mm_check();

struct spot {
	char* ptr;
	bool free;
	unsigned long size;
	struct spot prev;
	struct spot next;
};

static struct spot head;






#endif
