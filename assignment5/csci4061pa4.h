/*
 * csci4061pa4.h
 *
 *      Author: Abhijeet
 */

#ifndef CSCI4061PA4_H_
#define CSCI4061PA4_H_

char* file_getline(char* buffer, FILE* fp);

void decrypt(char* text);

void* child(void* args);

struct message
{
	int ID;
	int len;
	char payload[161];
};

#endif /* CSCI4061PA4_H_ */
