/**************************
 * maum.h -- the header file for maum.c 
 *
 *
 *
 *
 *************************/

#ifndef _MAIN_H_
#define _MAIN_H_

#define true 1
#define false 0
typedef int bool;


/*
This struct is a misnomer. it should be called Target. it houses
the name of it, the children below it (the dependencies), and 
the commands to be executed.

*/

struct dep
{
    char* name;
    struct lst *children;
    struct com *c;
};

void custmake_usage(char*);
void parse_file(char*);
void getCommands(struct dep*);



#endif
