/*
 * main.h
 *
 *      Author: Abhijeet
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <semaphore.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

#ifdef __unix__
# include <unistd.h>
#elif defined _WIN32
# include <windows.h>
#define sleep(x) Sleep(1000 * x)
#endif

#define MAX_CHARS_LINE 500
#define MAX_CLIENTS 1000

#endif /* MAIN_H_ */
