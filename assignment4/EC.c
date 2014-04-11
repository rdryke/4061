#include <pthread.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>



struct linkedList {
        char * path;
        struct linkedList * next;
};


int num_threads;
char * clientPath;
char * outputPath;
char * output;
struct linkedList * head = NULL;
struct linkedList * tail = NULL;
char * logPath;
char * decrypter(char*);
int logDes;
pthread_mutex_t qLock;
int qsize;
int currentSize;
int currentLine;
int nclients;

void printData()
{
	struct linkedList * current =  head;
	while (current != NULL)
	{
		printf("%s",current->path);
		current = current->next;
	}
}


FILE* file_open(char* filename) {
	FILE* fp = fopen(filename, "r");
	if(fp == NULL) {
		fprintf(stderr, "ERROR: while opening file %s, abort.\n", filename);
		exit(1);
	}
	return fp;
}

char* file_getline(char* buffer, FILE* fp) {
	buffer = fgets(buffer, 500, fp);
	return buffer;
}


int numberOfLines(char * filename)
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


void getDataFirst(char * filename)
{
	char* line = malloc(500*sizeof(char));
	FILE * file = file_open(filename);
	if ((line = file_getline(line,file)) != NULL && currentSize < qsize)
	{
		struct linkedList * temp =  malloc(sizeof(struct linkedList));
		head = temp;
		char * strtemp = malloc(strlen(line));
		char * tok = strtok(line, "\n");
		strcpy(strtemp, tok);
		head->path = strtemp;
		tail = head;
		currentLine++;
		pthread_mutex_lock(&qLock);
			currentSize++;
		pthread_mutex_unlock(&qLock);
	}
	
	while((line = file_getline(line,file)) != NULL && currentSize < qsize)
	{
		struct linkedList * temp =  malloc(sizeof(struct linkedList));
		char * strtemp = malloc(strlen(line));
		char * tok = strtok(line, "\n");
		strcpy(strtemp, tok);
		temp->path = strtemp;
		tail->next = temp;
		tail = temp;
		currentLine++;
		
		pthread_mutex_lock(&qLock);
			currentSize++;
		pthread_mutex_unlock(&qLock);
	}
	fclose(file);
	free(line);
	

}


void getDataSecond(char * filename)
{

	char* line = malloc(500*sizeof(char));
	FILE * file = file_open(filename);
	int i;
	for (i = 0; i<currentLine; i++)
	{
		line = file_getline(line,file);
	}
	
	while((line = file_getline(line,file)) != NULL && currentSize < qsize)
	{
		struct linkedList * temp =  malloc(sizeof(struct linkedList));
		char * strtemp = malloc(strlen(line));
		char * tok = strtok(line, "\n");
		strcpy(strtemp, tok);
		temp->path = strtemp;
		tail->next = temp;
		tail = temp;
		currentLine++;
		
		pthread_mutex_lock(&qLock);
			currentSize++;
		pthread_mutex_unlock(&qLock);
	}
	fclose(file);
	free(line);
	

}


void clientFunction(long threadID, char * fileName)
{

	int fileSize;
	int fileDes;
	char * text;
	char * decryptedText;
	int writeNum;
	int writeNum2;
	int writeFileDes;

	char log[strlen(fileName)+20];
	char log2[strlen(fileName)+30];
	struct stat fileStuff;

	stat(fileName, &fileStuff);
	fileSize = fileStuff.st_size;

	text = malloc(sizeof(char) * fileSize);
	fileDes = open(fileName, O_RDONLY, S_IRUSR);

	if (fileDes < 0)
	{
		writeNum2 = sprintf(log2, "Could not open file: %s\n", fileName);
		if(write(logDes, log2, writeNum2) < 0)
		{
			printf("Failed to write to log file\n");
			exit(-1);
		}
		return;
   	}
	if (read(fileDes, text, fileSize) < 0) 
	{
 	       printf("Could not read file: %s\n", fileName);
 	       exit(-1);
    	}
	
	decryptedText = malloc(sizeof(char) * fileSize);
	decryptedText = decrypter(text);

	writeNum = sprintf(log, "%s: %ld\n", fileName, threadID);
	if(write(logDes, log, writeNum) < 0)
	{
		printf("Failed to write to log file\n");
		exit(-1);
	}
	char * name = strrchr(fileName, '/');
	char * outputFileName = malloc(sizeof(char) * (strlen(name) + strlen(output)));
	strcpy(outputFileName, output);
	strcat(outputFileName, name);
	
	writeFileDes = open(outputFileName, O_CREAT | O_RDWR | O_APPEND | O_TRUNC, S_IWUSR | S_IRUSR);
	if (writeFileDes < 0)
	{
		printf("Could not open client output file\n") ;
		exit(-1);
	}
	if (write(writeFileDes, decryptedText, fileSize) < 0)
	{
		printf("Write to client output file failed\n");
		exit(-1);
	}
	close(writeFileDes);
	close(fileDes);
	free(text);
	free(decryptedText);
	free(outputFileName);
}

char * decrypter(char * text)
{
	int length = strlen(text);
	char * output = malloc(sizeof(char)*length);
	strcpy(output, text);
	int i;
	int cint;
	for (i = 0; i < length; i++)
	{
		cint = *(output + i);
		if ((65 <= cint && cint <= 88) || (97 <= cint && cint <= 120))
		{
			*(output + i) = cint + 2;
		}
		else if (cint == 89)
		{
			*(output + i) = 65;
		}
		else if (cint == 90)
		{
			*(output + i) = 66;
		}
		else if (cint == 121)
		{
			*(output + i) = 97;
		}
		else if (cint == 122)
		{
			*(output + i) = 98;
		}
	}
    return output;
	
}



void * pthreadFunction( void * s)
{
	int check;
	int i;
	char * fileName;
	while (head != NULL) 
	{
		check = 0;
		pthread_mutex_lock(&qLock);
        		if (head != NULL)
			{
				fileName = head->path;	
				head = head->next;
				check = 1;
				currentSize--;
			}
		pthread_mutex_unlock(&qLock);
		if (check == 1)
		{
			clientFunction((long) pthread_self(), fileName);
		}
    	}

    	return(0);	
}




int main ( int argc, char *argv[] )
{

	currentLine = 0;
	currentSize = 0;
	
	num_threads = 5;
	qsize = 7;
	if (argc == 5)
	{
		num_threads = atoi(argv[3]);
		qsize = atoi(argv[4]);
	}
	else if (argc != 3) 
	{
        	printf("ERROR: Invalid arguments.\nFormat is: ./retrieve clients.txt output [num_threads]\n");
        	return -1;
    	}
	clientPath = argv[1];
	outputPath = argv[2];
	output = (char *) malloc(strlen(outputPath) + 1);
	strcpy(output, outputPath);
	strcat(output, "/");
	logPath = (char *) malloc(strlen(output) + 7);
	strcpy(logPath, output);
	strcat(logPath, "log.txt");

	logDes = open(logPath, O_CREAT | O_RDWR | O_APPEND | O_TRUNC, S_IWUSR | S_IRUSR);
	if (logDes < 0) 
	{
		printf("Could not create log file\n");
        	return -1;
	}

	nclients = numberOfLines(clientPath);
	getDataFirst(clientPath);


	if (pthread_mutex_init(&qLock, NULL) != 0) 
	{
        	perror("mutex qLock failed\n");
        	return -1;
	}

	
	pthread_t threadArray[num_threads];
   	int i;
	
	

	   for (i = 0; i < num_threads; i++)
	{

	        pthread_create(&threadArray[i], NULL, pthreadFunction, (void *)i);
    	}

	int writeNum3;
	char log3[50];
	int printCheck = 0;
	while(currentLine != nclients)
	{
		if(printCheck == 0 && currentSize == qsize)
		{
			writeNum3 = sprintf(log3, "INFO: Queue full. Waiting to add more clients.\n");
			if(write(logDes, log3, writeNum3) < 0)
			{
				printf("Failed to write to log file\n");
				exit(-1);
			}
			printCheck = 1;
		}
		
		else if (currentSize < qsize)
		{
			getDataSecond(clientPath);
			printCheck = 0;
		}
			
	}

	for (i = 0; i < num_threads; i++)
	{
	        pthread_join(threadArray[i], NULL);
    	}
	close(logDes);
	free(output);
	free(logPath);
    	
}









