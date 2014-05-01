#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include "csci4061pa4.h"
#include <fcntl.h>
#include <unistd.h>


struct linkedList {		//simple linked list data structure for managing clients.
        char * text;
        struct linkedList * next;
};

struct linkedList * head = NULL;
struct linkedList * tail = NULL;


typedef struct message msg;
FILE* file_open(char* filename) //Used to open files.
{
	FILE* fp = fopen(filename, "r");
	if(fp == NULL)
	{
		fprintf(stderr, "ERROR: while opening file %s, abort.\n", filename);
		exit(1);
	}
	return fp;
}

char* file_getline(char* buffer, FILE* fp)  //reads a line of text from client files into a buffer
{
	buffer = fgets(buffer, 500, fp);
	return buffer;
}


int numberOfLines(char * filename)		//gets the number of lines in a file by counting '\n' characters
{
	int nlines = 0;
	char c;
	FILE * file = file_open(filename);

	while ( (c = fgetc(file)) != EOF )
	{
        	if ( c == '\n' )
		{
            		nlines++;
		}
    	}
	fclose(file);
	return nlines;
}


void getData(char * filename)			//Parses the "clients.txt" passed to the program and puts
{						//clients into the linked list data structure

	char* line = malloc(500*sizeof(char));
	FILE * file = file_open(filename);
	if ((line = file_getline(line,file)) != NULL)
	{
		struct linkedList * temp =  malloc(sizeof(struct linkedList));
		head = temp;
		char * strtemp = malloc(strlen(line));
		char * tok = strtok(line, "\n");	//this gets rid of trailing newline characters in text
		strcpy(strtemp, tok);
		head->text = strtemp;			//iterate to next element of linked list
		tail = head;
	}

	while((line = file_getline(line,file)) != NULL)	//continue above process until all clients are in the list
	{
		struct linkedList * temp =  malloc(sizeof(struct linkedList));
		char * strtemp = malloc(strlen(line));
		char * tok = strtok(line, "\n");
		strcpy(strtemp, tok);
		temp->text = strtemp;
		tail->next = temp;
		tail = temp;
	}
	fclose(file);			//we are done with the file, so close it and we are done with line, so free it.
	free(line);


}


int main(int argc, char *argv[])
{
    int sockfd;
    msg* m;
    int writeFileDes;
    struct sockaddr_in servaddr;

    if(argc != 4)
    {
        perror("ERROR: Invalid number of arguments, see usage in README.\n");
        return 1;
    }

    getData(argv[3]);

    char * outputFile = (char *) malloc(sizeof(argv[3]) + 11);
    if ((sprintf(outputFile, "%s.decrypted", argv[3])) < 0)
    {
	    perror("ERROR: Failed to make output file string.\n");
	    return 1;
    }

    int maxSize = (sizeof(int) * 2 + sizeof(char) * 161);

    m = malloc(maxSize);
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("ERROR: Could not create socket \n");
        return 1;
    }

    memset(&servaddr, '0', sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[2]));

    if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr)<=0)
    {
        perror("ERROR: inet_pton error occured\n");
        return 1;
    }

    if( connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
       perror("ERROR: Connect Failed \n");
       return 1;
    }

    if ((recv(sockfd, m, sizeof(msg), 0)) == -1)
    {
        perror("WARN: Failed to receive handshake mesage.\n");
	return 1;
    }
    else if (m->ID != 100)
    {
	perror("WARN: Server failed to send a handshake message.\n");
    }
    m->ID = 101;

    if ((send(sockfd, m, sizeof(msg), 0)) == -1)
    {
	perror("WARN: Failed to send handshake response message.\n");
	return 1;
    }
    writeFileDes = open(outputFile, O_CREAT | O_RDWR | O_APPEND | O_TRUNC, S_IWUSR | S_IRUSR);
    if (writeFileDes < 0)
    {
	    perror("ERROR:Could not open .decrypt file\n") ;
	    return 1;
    }

    struct linkedList * current = (struct linkedList *) malloc(sizeof(struct linkedList));
    current = head;

    while (current != NULL)
	    {
		    m->payload = current->text;
		    m->len = sizeof(current->text);
		    if ((send(sockfd, m, maxSize, 0)) == -1)
		    {
			    perror("WARN: Failed to send text.\n");
			    continue;
		    }
		    if ((recv(sockfd, m, maxSize, 0)) == -1)
		    {
			    perror("WARN: Failed to recieve dcrypted text.\n");
			    continue;
		    }
		    if (m->ID != 103)
		    {
		          perror("WARN: Server failed to decrypt message.\n");
			  continue;
		    }

		    if (write(writeFileDes, m->payload, m->len) < 0)	//write decrypted text into output file
		    {
			    perror("WARN: Write to .decrypt file failed\n");
			    continue;
		    }
		    current = current->next;
	    }

    m->ID = 104;
    m->len = 104;
    m->payload = "";

    if ((send(sockfd, m, sizeof(msg), 0)) == -1)
    {
	perror("WARN: Failed to send handshake response message.\n");
	return 1;
    }

    close(writeFileDes);	//clean up for closing opened files and freeing variables that are no longer needed
    free(outputFile);

    return 0;
}
