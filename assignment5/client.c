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



typedef struct message msg;


/*

void printData()	//used for testing purposes.
{
	struct linkedList * current =  head;
	while (current != NULL)
	{
		if (line == NULL)
		{
			printf("\n");
			current = current->next;
			continue;
		}
		printf("%s\n",line);
		current = current->next;
	}
}
*/
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

FILE* file_open_append(char* filename) //Used to open files.
{
	FILE* fp = fopen(filename, "a+");
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

/*
void getData(char * filename, int n)			//Parses the "clients.txt" passed to the program and puts
{						//clients into the linked list data structure
	if (n == 0)
	{

	}
	else if (n == 1)
	{
		char* line = malloc(161*sizeof(char));
		FILE * file = file_open(filename);
		if ((line = file_getline(line,file)) != NULL)
		{
			struct linkedList * temp =  malloc(sizeof(struct linkedList));
			head = temp;
			char * strtemp = malloc(strlen(line));
			char * tok = strtok(line, "\n");	//this gets rid of trailing newline characters in text
			strcpy(strtemp, tok);
			head->text = strtemp;			//iterate to next element of linked list
			head->next =NULL;
		}
	fclose(file);			//we are done with the file, so close it and we are done with line, so free it.
	free(line);

	}
	else
	{
		char* line = malloc(500*sizeof(char));
		FILE * file = file_open(filename);
		if ((line = file_getline(line,file)) != NULL)
		{
			struct linkedList * temp =  malloc(sizeof(struct linkedList));
			char * strtemp = malloc(strlen(line));
			int l = strlen(line);
			if (l == 1)
			{
				head = temp;
				head->text = "";			//iterate to next element of linked list
				tail = head;
			}
			else
			{
				char * tok = strtok(line, "\n");	//this gets rid of trailing newline characters in text
				strcpy(strtemp, tok);
				head = temp;
				head->text = strtemp;			//iterate to next element of linked list
				tail = head;
			}
		//	printf("|%s|\n",temp->text);
		}

		while((line = file_getline(line,file)) != NULL)	//continue above process until all clients are in the list
		{
			struct linkedList * temp =  malloc(sizeof(struct linkedList));
			char * strtemp = malloc(strlen(line));
			int l = strlen(line);
			if (l == 1)
			{
				temp->text = "";			//iterate to next element of linked list
				tail->next = temp;
				tail = temp;
			}
			else
			{
				char * tok = strtok(line, "\n");
				strcpy(strtemp, tok);
				temp->text = strtemp;
				tail->next = temp;
				tail = temp;
			}
			tail->next = NULL;
			//printf("|%s|\n",temp->text);
		}
		fclose(file);			//we are done with the file, so close it and we are done with line, so free it.
		free(line);

	}

}
*/

int main(int argc, char *argv[])
{
    int sockfd;		//initalize variables
    msg* m;
    int writeFileDes;
    struct sockaddr_in servaddr;
    char * currentFile;
    char * line;
    int i, j;
    int maxSize;
    int lines;
    char * outputFile;
    FILE * f;
    if(argc < 4)
    {
        perror("ERROR: Invalid number of arguments, see usage in README.\n");
        return 1;
    }
    maxSize = (sizeof(int) * 2 + sizeof(char) * 161);

    m = malloc(maxSize);
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)	//set up socket
    {
        perror("ERROR: Could not create socket \n");
        return 1;
    }

    memset(&servaddr, '0', sizeof(servaddr));		//set up server address recognition

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[2]));
    char* host;

    if(strcmp("localhost", argv[1]) == 0)	//handle localhost case
	host = "127.0.0.1";
    else
	host = argv[1];
    if(inet_pton(AF_INET, host, &servaddr.sin_addr)<=0)
    {
	perror("ERROR: inet_pton error occured\n");
	return 1;
    }

    if( connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)	//connect with server
    {
       perror("ERROR: Connect Failed \n");
       return 1;
    }

    if ((recv(sockfd, m, sizeof(msg), 0)) == -1)		//listen for handshake message
    {
        perror("ERROR: Failed to receive handshake mesage.\n");
	m->ID = 105;
	if((send(sockfd, m, sizeof(msg), 0)) == -1)
		perror("ERROR: Failed to send error message.\n");
	return 1;
    }
    if (m->ID == 105) {
	perror("Server sent error message.");
	printf("Payload of error message received from server: %s", m->payload);
	}
    if (m->ID != 100)
    {
	perror("ERROR: Server failed to send a handshake message.\n");
	m->ID = 105;
	if((send(sockfd, m, sizeof(msg), 0)) == -1)
		perror("ERROR: Failed to send error message.\n");
	return 1;
    }
    m->ID = 101;
    m->len = 0;

    if ((send(sockfd, m, sizeof(msg), 0)) == -1)		//send handshake response message
    {
	perror("ERROR: Failed to send handshake response message.\n");
	m->ID = 105;
	if((send(sockfd, m, sizeof(msg), 0)) == -1)
		perror("ERROR: Failed to send error message.\n");
	return 1;
    }



    for (i = 3; i < argc; i++)		//Extra credit loop for multiple files
    {

        outputFile = (char *) malloc(strlen(argv[i]) + 11);
        currentFile = (char *) malloc(strlen(argv[i]));

	strcpy(currentFile,argv[i]);

	lines = numberOfLines(currentFile);		//for parsing the lines in the files
	if ((sprintf(outputFile, "%s.decrypted", currentFile)) < 0)
   	{
		perror("ERROR: Failed to make output file string.\n");
		m->ID = 105;
		if((send(sockfd, m, sizeof(msg), 0)) == -1)
			perror("ERROR: Failed to send error message.\n");
		return 1;
    	}

	if (lines == 0)		//handle empty client file case
	{

    		writeFileDes = open(outputFile, O_CREAT | O_RDWR | O_APPEND | O_TRUNC, S_IWUSR | S_IRUSR);	//open file to write to
    		if (writeFileDes < 0)
    		{
			perror("ERROR:Could not open .decrypt file\n");
			m->ID = 105;
			if((send(sockfd, m, sizeof(msg), 0)) == -1)
				perror("ERROR: Failed to send error message.\n");
			return 1;
    		}
		continue;

	}


    	writeFileDes = open(outputFile, O_CREAT | O_RDWR | O_APPEND | O_TRUNC, S_IWUSR | S_IRUSR);	//open file to write to
    	if (writeFileDes < 0)
    	{
		perror("ERROR:Could not open .decrypt file\n");
		m->ID = 105;
		if((send(sockfd, m, sizeof(msg), 0)) == -1)
			perror("ERROR: Failed to send error message.\n");
		return 1;
    	}


	line = (char *) malloc(161*sizeof(char));
	f = file_open(currentFile);

	while((line = file_getline(line,f)) != NULL)	//continue above process until all clients are in the list
	{
		if (line == NULL)
		{
			write(writeFileDes, "\n", 1);
			continue;
		}
		m->ID = 102;				//send payload

		strcpy(m->payload, line);
		m->len = strlen(line);
		if ((send(sockfd, m, maxSize, 0)) == -1)
		{
			perror("WARN: Failed to send text.\n");
			continue;
		}
		if ((recv(sockfd, m, maxSize, 0)) == -1)	//listen for decrypted text
		{
			perror("WARN: Failed to recieve dcrypted text.\n");
			continue;
		}
		if (m->ID == 105) {
		perror("Server sent error message.");
		printf("Payload of error message received from server: %s", m->payload);
		}
		if (m->ID != 103)
		{
			perror("WARN: Server failed to decrypt message.\n");
			continue;
		}

//		char nline[m->len + 1];
//		strcpy(nline,m->payload);
//		strcat(nline, "\n");

		if (write(writeFileDes, m->payload, m->len) < 0)	//write decrypted text into output file
		{
			perror("WARN: Write to .decrypt file failed\n");
			continue;
		}
	//	free(nline);
	}
	close(writeFileDes);	//clean up for closing opened files and freeing temporary variables.
	fclose(f);	//clean up for closing opened files and freeing temporary variables.
	free(outputFile);
	free(currentFile);
	free(line);
    }

    m->ID = 104;	//end of request
    m->len = 0;

    if ((send(sockfd, m, sizeof(msg), 0)) == -1)
    {
	perror("ERROR: Failed to send end of request message.\n");
	m->ID = 105;
	if((send(sockfd, m, sizeof(msg), 0)) == -1)
		perror("ERROR: Failed to send error message.\n");
	return 1;
    }



    return 0;
}
