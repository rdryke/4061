/*******************************
 * main.c
 *
 * Source code for main
 *
 ******************************/

#include "util.h"
#include "main.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/*********
 * Simple usage instructions
 *********/
void custmake_usage(char* progname) {
	fprintf(stderr, "Usage: %s [options] [target]\n", progname);
	fprintf(stderr, "-f FILE\t\tRead FILE as a custMakefile.\n");
	fprintf(stderr, "-h\t\tPrint this message and exit.\n");
	fprintf(stderr, "-n\t\tDon't actually execute commands, just print them.\n");
	exit(0);
}



struct com				//struct for commands, its in a linked list style structure
{
    char* str;
    struct com *next;
};

struct lst				//struct for list of children also in linked list style
{
    char* str;
    struct lst *next;
};

struct targets				//struct for the targets from the file. Struct dep *d points 
					//to the data structure that houses a target, NOT the dependencies 
					//of a specific target. The struct dep is defined in main.h
{
    struct dep *d;
    struct targets *next;
};






static struct targets *head;		//global variable to hold the front of the list of targets

/*The function below is for finding the struct dep from the string of a target. 
the input str is the name of the target, the int check is to see 
if this is a call from a previous target or a call from the 
command line. if its from the command line, and it doesnt find it, 
an error is reported and the process ends. If it is from a previous 
target, then the variable found is used.  The variable found 
tells the caller that no such target was found and prints a warning.
The rest of dependencies still run though.
*/

struct dep * findDep(char* str, int check, int *found)  
{
	struct targets *temp = head;
	while (temp->next != NULL && strcmp(temp->d->name, str) != 0)
	{
		temp = temp->next;
	}
	if (strcmp(temp->d->name, str) != 0)
	{
		if (check == 0)
		{
			printf("ERROR: No such target\n");
			exit(-1);
		}
		else
		{
			printf("WARN: target %s does not exist\n", str);
			struct dep *empty = (struct dep*)malloc(sizeof(struct dep)); 
			*found = 0;
			return empty;
		}
	}
	else
	{
		return temp->d;
	}
	
}

/****************************** 
 * this is the function that, when given a proper filename, will
 * parse the custMakefile and read in the targets and commands
 ***************/
void parse_file(char* filename) {

	char* line = malloc(160*sizeof(char));
	FILE* fp = file_open(filename);
	

	struct com *currentCommands = (struct com*)malloc(sizeof(struct com));		//initialize list of commands.
	struct targets *master = (struct targets*)malloc(sizeof(struct targets));	//initialize list of targets
	
	while((line = file_getline(line, fp)) != NULL) {				//goes through every line of the file

		if (strstr(line, ":") == NULL)						//checks if the line is a target
			{
				if (strcmp(line, "end\n") != 0 || strcmp(line, "") != 0) //checks for end of line or "end"strings
				{
					
					if (currentCommands->str == NULL)		//checks if list of commands is initialized
					{
						char* strng4 = (char*)malloc(sizeof(line) + 160) ;	// the 160 comes from the max 
													//size of a line, so the string 
													//will not be cut off ever. 
						strcpy(strng4, line);
						char* pch = strtok(strng4, "\n");
						currentCommands->str = pch;
					}
					else
					{
						struct com *temp = (struct com*)malloc(sizeof(struct com));
						struct com *current = currentCommands;	//create a temporary instance of the list for iteration
						while (current->next != NULL)		//iterates through list of commands to last element
						{
							current = current->next;
						}
						char* strng5 = (char*)malloc(sizeof(line) + 160) ;
						strcpy(strng5, line);
						char* pch = strtok(strng5, "\n");
						temp->str = pch;
						current->next = temp;			//appends the next command to the list of commands
						
					}
				}
				else
				{
					currentCommands = NULL;				//no more commands for that target
				}
				
			}
		else									//if its a target
			{
				char* pch = strtok (line,":");				//tokenize after the target name
				
				struct dep *bla = (struct dep*)malloc(sizeof(struct dep));
				char* strng = (char*)malloc(sizeof(pch)) ;
				strcpy(strng, pch);
				bla->name = strng;					//initialize dependencies in the struct
				
				struct lst *chldrn = (struct lst*)malloc(sizeof(struct lst));
				
				struct com *cmmnds = (struct com*)malloc(sizeof(struct com));
				bla->children = chldrn;					//put in list of children for tree representation
				bla->c = cmmnds;					//put in commands for the target
				pch = strtok (NULL, " ");
				while(pch != NULL)					//iterate through each dependent target
				{
					if (chldrn->str == NULL)			//put dependencies as children for tree representation
					{
						char* strng2 = (char*)malloc(sizeof(pch)) ;
						strcpy(strng2, pch);
						chldrn->str = strng2;
					}
					else
					{
						struct lst *temp = (struct lst*)malloc(sizeof(struct lst));
						struct lst *current = chldrn;
						while (current->next != NULL)
						{
							current = current->next;

						}
						char* strng3 = (char*)malloc(sizeof(pch)) ;
						strcpy(strng3, pch);
						temp->str = strng3;
						current->next = temp;
					}
					pch = strtok (NULL, " ");
				}
				if (master->d == NULL)					//initialize head of the list of targets
				{
					master->d = bla;
				}
				else
				{
					struct targets *temp = (struct targets*)malloc(sizeof(struct targets));
					struct targets *current = master;
					while (current->next != NULL)
					{
						current = current->next;		//iterate through targets
					}
					temp->d = bla;					//append current target
					current->next = temp;
				}
			

				currentCommands = cmmnds;
			}



	

	}
	fclose(fp);
	free(line);

	head = master;									//set head as the first target, or rather, the root node of the tree

}

void getCommands(struct dep *a)		//function used to go through and execute all commands recursively. 
{
	while (a != NULL && a->children != NULL && a->children->str != NULL && a->children->next != NULL)
	{
		
		char* pch = strtok(a->children->str, "\n");
		if(pch != NULL)
		{
			int b = 1;
			struct dep *temp = findDep(pch, 1, &b);
			if (b != 0)
			{
			getCommands(temp);		//from the recursive structure, the lowest node in
			}
			a->children = a->children->next;
		}
	}
	if(a != NULL && a-> children != NULL &&  a->children->str != NULL)
	{
	char* pch = strtok(a->children->str, "\n");
		if(pch != NULL)
		{
			int b = 1;
			struct dep *temp = findDep(pch, 1, &b);
			if (b != 0)
			{
			getCommands(temp);
			}
		}
	}
	while (a != NULL && a->children != NULL && a->c->next != NULL)
	{
		if(strcmp(a->c->str, "end\n") != 0 && a->c->str != NULL && strcmp(a->c->str, "end") != 0) 	//checks for "ends"
		{
			int leadingWhitespaces = strspn(a->c->str, "	 ");	//removes whitespace from output by finding how many
			a->c->str += leadingWhitespaces;			//whitespace characters are present and incrementing pointer
			
			char* commandLineArguments;
			char* pointerToFirstWhiteSpace = strchr(a->c->str, ' ');
			if (pointerToFirstWhiteSpace != NULL)
			{
				commandLineArguments = pointerToFirstWhiteSpace + 1;
				char *endOfString;
				endOfString = strchr(a->c->str, ' ');
				*endOfString = '\0';
			}
			int pid = fork();	//forks to execute commands in new processes
			if (pid == -1)
			{
				printf("ERROR:Fork call failed\n");
				exit(-1);
			}
			else if (pid == 0)	//if new process, execute command
			{
				execlp(a->c->str,a->c->str,commandLineArguments,(char*)NULL);
			}
			else	//otherwise wait until child commands are finished
			{
				wait();
			}	
			
		}
		a->c = a->c->next;
			
	}
	
	if (a != NULL && a->children != NULL && a->c->next != NULL && strcmp(a->c->str, "end") != 0)	//same as above. The checks cause us to be off by one in the while
	{								//so this compensates
		int leadingWhitespaces = strspn(a->c->str, "	 ");
		a->c->str += leadingWhitespaces;
		char* commandLineArguments;
		char* pointerToFirstWhiteSpace = strchr(a->c->str, ' ');
		if (pointerToFirstWhiteSpace != NULL)
		{
			commandLineArguments = pointerToFirstWhiteSpace + 1;
			char *endOfString;
			endOfString = strchr(a->c->str, ' ');
			*endOfString = '\0';
		}
		int pid = fork();
		if (pid == -1)
		{
			printf("ERROR:Fork call failed\n");
			exit(-1);
		}
		else if (pid == 0)
		{
			execlp(a->c->str,a->c->str,commandLineArguments,(char*)NULL);
		}
		else
		{
			wait();
		}	
	}

}

void printGetCommands(struct dep *a)		//function to print all commands for calls with -n
{
	while (a != NULL && a->children != NULL && a->children->str != NULL && a->children->next != NULL)	//iterate through children of the node
	{											//designated as the root by the call
		
		char* pch = strtok(a->children->str, "\n");
		if(pch != NULL)
		{
			int b = 1;
			struct dep *temp = findDep(pch, 1, &b);
			if (b != 0)
			{
				printGetCommands(temp);
			}
				a->children = a->children->next;	//move on to next child of the node in question
		}
	}
	if(a != NULL && a->children->str != NULL)				
	{
	char* pch = strtok(a->children->str, "\n");		//takes care of the last command since		
		if(pch != NULL)					//the loop will be off by one
		{
			int b = 1;
			struct dep *temp = findDep(pch, 1, &b);
			if (b != 0)
			{
				printGetCommands(temp);
			}
		}
	}
	while (a != NULL && a->children != NULL && a->c->next != NULL)
	{
		if(strcmp(a->c->str, "end\n") != 0 && a->c->str != NULL && strcmp(a->c->str, "end") != 0)
		{
			int leadingWhitespaces = strspn(a->c->str, "	 ");
			a->c->str += leadingWhitespaces;
			printf("%s\n", a->c->str);
			
		}
		a->c = a->c->next;
			
	}
	
	if (a != NULL && a->c->next != NULL && strcmp(a->c->str, "end") != 0)	//handles whitespace
	{
		int leadingWhitespaces = strspn(a->c->str, "	 ");	//finds needed offset
		a->c->str += leadingWhitespaces;			//increments pointer to first non-whitespace character
		printf("%s\n", a->c->str);	//prints the command with no whitespace
	}

}

int main(int argc, char* argv[]) {
	// Declarations for getopt
	extern int optind;
	extern char* optarg;
	int ch;
	char* format = "f:hn";
	


	// Variables you'll want to use
	char* filename = "custMakefile";
	bool execute = true;

	// Use getopt code to take input appropriately (see section 3).
	while((ch = getopt(argc, argv, format)) != -1) {
		switch(ch) {
			case 'f':
				filename = strdup(optarg);
				break;
			case 'n':
				execute = false;
				break;
			case 'h':
				custmake_usage(argv[0]);
				break;
		}
	}
	argc -= optind;
	argv += optind;

	

	parse_file(filename);


	if (execute)		//standard run or with -f
	{
		if (argc == 1)	//if target is passed as input
		{

			int b = 1;
			char* firstTarget = argv[0];		//gets first target from input
			struct dep *temp = findDep(firstTarget, 0, &b);
			getCommands(temp);			//use command executing function on the passed target


	
		}
		else if (argc > 1)
		{
			printf("ERROR: Too many arguments. Please use only one target\n");
			exit(-1);
		}
		else
		{
			int b = 1;
			struct dep *temp = findDep(head->d->name, 0, &b);	//if no input, first target is the first target in the file
			getCommands(temp);				//use command executing function with the first target in the file
		}
	}
	else			//for a call with -n
	{
		if (argc == 1)	//if -n call with an argument
		{

			int b = 1;
			
			char* firstTarget = argv[0];		//sets passed argument as the starting point in the tree
			struct dep *temp = findDep(firstTarget, 0, &b);
			printGetCommands(temp);			//use printing function


	
		}
		else if (argc > 1)
		{
			printf("ERROR: Too many arguments. Please use only one target\n");
			exit(-1);
		}
		else		//-n call with no argument
		{
			int b = 1;
			struct dep *temp = findDep(head->d->name, 0, &b);	//starting point in tree is the first target in the file
			printGetCommands(temp);				//use printing function
		}

	}

	
	return 0;
}






