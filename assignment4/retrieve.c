#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>

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
char * decrypter(char*, int);
pthread_mutex_t qLock;
pthread_mutex_t logLock;
int logDes;


void printData()
{
	struct linkedList * current =  head;
	while (current != NULL)
	{
		printf("|%s|",current->path);
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


void getData(char * filename, int n)
{
	int *a = (int *) malloc(sizeof(int) * n);
	char* line = malloc(500*sizeof(char));
	FILE * file = file_open(filename);
	if ((line = file_getline(line,file)) != NULL)
	{
		struct linkedList * temp =  malloc(sizeof(struct linkedList));
		head = temp;
		char * strtemp = malloc(strlen(line));
		strcpy(strtemp, line);
		char * nl;
		if ((nl = strchr(strtemp, '\n')) != NULL)
		{
			*nl = '\0';
		}
		head->path = strtemp;
		tail = head;
	}
	
	while((line = file_getline(line,file)) != NULL)
	{
		struct linkedList * temp =  malloc(sizeof(struct linkedList));
		char * strtemp = malloc(strlen(line));
		strncpy(strtemp, line, strlen(line));
		char * nl;
		if ((nl = strchr(strtemp, '\n')) != NULL)
		{
			*nl = '\0';
		}
		temp->path = strtemp;
		tail->next = temp;
		tail = temp;
	}
	fclose(file);
	free(line);
	

}


void clientFunction(long threadID)
{

	int fileSize;
	int fileDes;
	char * fileName;
	char * text;
	char * decryptedText;
	int writeNum;
	int writeFileDes;

	char log[strlen(fileName)+20];
	struct stat fileStuff;
	
	pthread_mutex_lock(&qLock);

		fileName = head->path;	
		head = head->next;

	pthread_mutex_unlock(&qLock);

	stat(fileName, &fileStuff);
	fileSize = fileStuff.st_size;

	text = malloc(sizeof(char) * fileSize);
	fileDes = open(fileName, O_RDONLY, S_IRUSR);

	if (fileDes < 0)
	{
		printf("Could not open file: %s\n", fileName);
        	exit(-1);
   	}
	if (read(fileDes, text, fileSize) < 0) 
	{
 	       printf("Could not read file: %s\n", fileName);
 	       exit(-1);
    	}
	
	decryptedText = malloc(sizeof(char) * fileSize);
	decryptedText = decrypter(text, fileSize);

	
	writeNum = sprintf(log, "%s: %ld\n", fileName, threadID);
	if(write(logDes, log, writeNum) < 0)
	{
		printf("Failed to write to log file\n");
		exit(-1);
	}
	char * name = strrchr(fileName, '/');
	char * outputFileName = malloc(sizeof(char) * (strlen(name) + strlen(output)));
printf("%s\n", outputFileName);
	strcpy(outputFileName, output);
	strcat(outputFileName, name);
	
	writeFileDes = open(outputFileName, O_CREAT | O_RDWR | O_APPEND | O_TRUNC, S_IWUSR | S_IRUSR);
	if (writeFileDes < 0)
	{
		printf("Could not open client output file\n") ;
	}
	if (write(writeFileDes, decryptedText, fileSize) < 0)
	{
		printf("Write to client output file failed\n");
	}

	
}

char * decrypter(char * text, int length)
{
	int i;
	int k;
	char * result = malloc(sizeof(char)*length);
	char c;
	for (i = 0; i < length; i++) 
	{
		c = result[i];
		

		if(isalpha(c))
		{
			if(c == 'y')
			{
				result[i] = 'a';
			}
			else if(c == 'z')
			{
				result[i] = 'b';
			}
			else if(c == 'Y')
			{
				result[i] = 'A';
			}
			else if(c == 'Z')
			{
				result[i] = 'B';
			}
			else
			{
				*(result + i) = *(result + i) + 2;
			}
		}

    	}
	return result;
	
}



void * pthreadFunction()
{
	while (head != NULL) 
	{
        	if (head != NULL)
		{
			clientFunction((long) pthread_self());
		}
    	}

    	return(0);	
}




int main ( int argc, char *argv[] )
{
	int nclients;
	num_threads = 5;
	if (argc == 4)
	{
		num_threads = atoi(argv[3]);
	}
	else if (argc != 3) 
	{
        	printf("ERROR: Invalid arguments.\nUsage: ./retrieve clients.txt output [num_threads]\n");
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
	getData(clientPath, nclients);
printData();
	if (pthread_mutex_init(&qLock, NULL) != 0) 
	{
        	perror("mutex qLock failed\n");
        	return -1;
	}

	if (pthread_mutex_init(&logLock, NULL) != 0) 
	{
        	perror("mutex logLock failed\n");
        	return -1;
	}
	
	pthread_t threadArray[num_threads];
   	int i;
    	for (i = 0; i < num_threads; i++)
	{

	        pthread_create(&threadArray[i], NULL, pthreadFunction, NULL);
    	}

	for (i = 0; i < num_threads; i++)
	{
	        pthread_join(threadArray[i], NULL);
    	}

}









