#include "mm.h"
#include <stdio.h>
#include <stdlib.h>

 
int main() 
{ 
 char *A, *B, *C, *D; 
 unsigned long free_num; 
 
 mm_init(10000); //create the memory pool and initialize data structure 
 
 A= mm_alloc(10); 

 B= mm_alloc(1000);  
 C= mm_alloc(200); 
 
 mm_free(B); 
 D=mm_alloc(20); 
 mm_free(C); //After this block is freed, it will be coalesced with 

 
 mm_free(D); //Trying to free the memory space pointed by an invalid pointer. 
 
 //leaks=mm_check(); 
 //printf("Total memory leaks:%lu\n", leaks); 
 // leaks=2, A,D are not freed 
 
 mm_end(&free_num); 
 printf("free blocks number:%lu\n", free_num); 
 //free_num=1 
 
 return 0; 
} 
