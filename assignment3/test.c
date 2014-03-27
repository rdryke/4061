#include "mm.h"
#include <stdio.h>
int main()
{
char *A, *B, *C, *D, *E;
unsigned long free_num, leaks;
mm_init(10000); //create the memory pool and initialize data structure
A= mm_alloc(10); 
B= mm_alloc(1000); C= mm_alloc(200); 
mm_free(B); 
D=mm_alloc(20); 
mm_free(C); //After this block is freed, it will be coalesced with 
int i=0;
for (i=0;i<=20;i++){
mm_assign(D+i,(char)i); //When i=20, should print “Buffer Overflow: Try to

}
mm_free(D+10); //Trying to free the memory space pointed by an invalid pointer. 
E = mm_alloc(20000);
leaks=mm_check();
printf("Total memory leaks:%lu\n", leaks);
// leaks=2, A,D are not freed
mm_end(&free_num);
printf("free blocks number:%lu\n", free_num);
//free_num=1
return 0;
}
