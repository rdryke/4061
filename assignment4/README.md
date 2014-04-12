CSCI 4061: Introduction to Operating Systems -- Programming Assignment 4
Authors: Jordan Anderson and Rob Dryke
Machine(s) used: This code was compiled and tested on kh4250-03.cselabs.umn.edu

CODE ORGANIZATION:
	This program uses multiple threads to emulate the completion of multiple client requests to a server. The program reads from a file with all client names to assemble a list of clients, then generates a certain number of threads to go into each client file and perform the requested process (in this case of this problem, decryption). Pthreads and mutex synchronization techniques are used to synchronize the main program with generated threads and avoid unexpected errors. 
	This program uses many auxiliary functions to keep things organized. The functions used and their behavior are as follows

int main(int argc, char *arg[v]):
	This is the main function that takes command line input (see usage instructions). This function performs a lot of set up (creating files, initializing variables, getting the number of clients, etc.) and proceeds to call on auxiliary functions and then generate threads and implement synchronization techniques.

FILE* file_open(char* filename):
	This function is self explanatory, it is just an auxiliary function to open a file and return the file pointer.

char* file_getline(char* buffer, FILE* fp):
	This function is straightforward and just reads a line of text into a buffer. This is used for the actual client files that are guaranteed to be only one line.

int numberOfLines(char * filename):
	This function returns the number of lines in the file filename by counting '\n' characters.

void getData(char* filename):
	This function is called by the main function to parse through the file containing the client names (see usage instructions) and put every client listed in the file into a linked list data structure.

clientFunction(long threadID, char * fileName):
	This is the function thateach thread goes into via pthreadFunction (see below). This function writes the text from filename into a buffer and then sends that buffer to decrypter() (see below) to decrypt the text. It then outputs the decrypted text with the threadID formatted as per the problem description.

char * decrypter(char * text):
	This function takes a string and decrypts it based on the rules given in the assignment. All letters have their numerical ASCII values shifted up by two, 'y', 'z', 'Y', 'Z' wrap around to 'a', 'b', 'A', 'B' respectively. Non alphabetical characters are left as they are. The function returns the decrypted string.

void * pthreadFunction(void * s):
	This function is used for all threads generated in the main function. It goes through the list of clients, using mutex locks to handle syncrhonization, and having each thread, after selecting a client that needs service,into clientFunction() for that client.

POTENTIAL ERRORS:
Most errors will be reported by appropriate error messages.Some more common potential errors are the result of passing invalid filepaths or a client.txt that has nonexistant clients, etc.

USAGE INSTRUCTIONS: 
	This code can be compiled with the provided makeFile or with a command line instruction of: "gcc -pthread  -o retrieve retrieve.c" 
This program takes two required arguments and one optional argument. It can be called from the command line by "./retrieve clients.txt output num-threads"

clients.txt is a required argument that is the path of the input file. That is, clients.txt should be the absolute path to a text file, all of whose lines are valid filepaths for clients that need decryption.

output is a required argument that is the absolute path of the directory in which the program should output its results (the decrypted strings and other information)

num-threads is an optional argument that determines the maximum number of threads the server can use at one time to handle requests. If this argument is not passed, the default number of threads allowed for the server is 5.



