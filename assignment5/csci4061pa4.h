/*
 * csci4061pa4.h
 *
 *      Author: Abhijeet
 */

#ifndef CSCI4061PA4_H_
#define CSCI4061PA4_H_

char* file_getline(char* buffer, FILE* fp);

int decrypt(char* in, char * out, int n);

void* child(void* args);

struct message
{
	int ID;
	int len;
	char * payload;
};

#endif /* CSCI4061PA4_H_ */
