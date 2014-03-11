/* 
 * File:   main.cpp
 * Author: Rob
 *
 * Created on March 6, 2014, 4:25 PM
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <inttypes.h>


/*
 * 
 */

/*
 char * makeCommandArgs(something pipe, char * string)
{
    
}
 */




void getstring(char * dir_path, char * search_string) {
    //initialize array for pipe	
    DIR *dir_ptr; //initialize pointer for current dirent
    struct dirent *current;

    dir_ptr = opendir(dir_path); //open dir and set pointer
    if (dir_ptr == NULL) {
        printf("ERROR: Could not open directory. Exiting now\n");
        exit(-1);
    }

    current = readdir(dir_ptr); //move pointer to first item in dir

    while (current != NULL) {
        int fd[2];
        if (pipe(fd) == -1) { //make pipe
            printf("ERROR: Failed to pipe. Exiting now\n");
            exit(-1);
        }
        if (strcmp(current->d_name, ".") == 0 || strcmp(current->d_name, "..") == 0) {
            current = readdir(dir_ptr);
            continue;
        }

        char * filepath = (char *) malloc(sizeof (char) * 257);
        strcpy(filepath, dir_path);
        strcat(filepath, "/");
        strcat(filepath, current->d_name);


        struct stat statbuf;

        if ((lstat(filepath, &statbuf))) //set statuses of file pointer and put in status buffer
        {
            perror("Error while opening the file.\n");
            exit(EXIT_FAILURE);
        } else {

            if (S_ISLNK(statbuf.st_mode)) //if current points to a link
            {

                current = readdir(dir_ptr); //skip
            } else if (S_ISDIR(statbuf.st_mode)) //if file pointer is pointing to a directory
            {
                int fp = open("log.txt", O_WRONLY | O_APPEND);
                close(1);
                dup(fp);
                //FILE *log = freopen("log.txt", "a", stdout);
                printf("%s\n", filepath);
                close(fp);


                getstring(filepath, search_string); //recurse in directory being pointed to
            } else { //if current points to a regular file
                struct stat st;
                stat(filepath, &st);
                int size = st.st_size;
                ssize_t bytes_read;

                int childpid = fork(); //fork child process

                if (childpid) //parent code writes text from file into pipe
                {
                    int fp = open("log.txt", O_WRONLY);
                close(1);
                dup(fp);
                //FILE *log = freopen("log.txt", "a", stdout);
                printf("%s\n", filepath);
                close(fp);

                    close(fd[0]);
                    char * contents = (char *) malloc(sizeof (char) * size);


                    int checkOpen = open(filepath, O_RDONLY | O_APPEND);
                    if (checkOpen < 0) {
                        printf("ERROR: Failed to open file. Exiting now\n");
                        exit(-1);
                    }
                    bytes_read = read(checkOpen, contents, size);
                    if (size != (int) bytes_read) {
                        printf("ERROR: Failed reading file. Exiting now\n");
                        exit(-1);
                    }

                    ssize_t written = write(fd[1], contents, size);

                    close(checkOpen);
                    close(fd[1]);
                    wait();

                }

                else //child code
                {


                    close(fd[1]);
                    //char * rbuf = (char *) malloc(sizeof(char) * size);
                    //ssize_t bytesReadPipe = read(fd[0], rbuf, size);




                    close(0);
                    dup(fd[0]);
                    close(fd[0]);
                    int fileo = open("out.txt", O_WRONLY | O_APPEND);
                    close(1);
                    dup(fileo);
                    //FILE *out = freopen("out.txt", "a", stdout);
                    
                    printf("bla");
                    char * const args[] = {"grep", search_string, 0};
                    execvp("/bin/grep", args); //call grep with input from pipe to print
                    close(fileo);



                }

            }


            current = readdir(dir_ptr); //move on to next entry
        }

    }


}

int main(int argc, char** argv) {
if (argc != 3)
{
printf("error\n");
exit(-1);
}

FILE * ftemp = fopen("out.txt", "w");
fclose(ftemp);

FILE * ftemp2 = fopen("log.txt", "w");
fclose(ftemp2);

char * folder = argv[1];
char * search = argv[2];
    getstring(folder, search);







    return 0;
}
