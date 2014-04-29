/*
 ============================================================================
 Name        : csci4061pa4.c
 Author      : Abhijeet Gaikwad
 Description : PA4 in C, Ansi-style
 ============================================================================
 */

#include "main.h"
#include "csci4061pa4.h"
#include "queue.h"

#define EXTRACT_FILE_LEN(client) ((strrchr(client, '/')) ? strlen(strrchr(client, '/')) : strlen(client))

//shared queue
queue_t *q;

typedef struct message msg;

//log mutex
static pthread_mutex_t stdout_mutex;

//semaphore to count number of clients in queue
sem_t sem;


//output directory
char* output_dir = NULL;

char* file_getline(char* buffer, FILE* fp) {
	buffer = fgets(buffer, MAX_CHARS_LINE, fp);
	return buffer;
}

int decrypt(char* in, char * out, int n)
{
	char c, out_char[n];
	int i;

	if (n == 0) {
		return 2;
	}

	for (i = 0; i < n; i++)
	{
		c = (char) in[i];
		if (c == 'y' || c == 'z' || c == 'Y' || c == 'Z')
		{
			out_char[i] = c - 24;
		}
		else if (('a' <= c && 'x' >= c) || (('A' <= c && 'X' >= c)))
		{
			out_char[i] = c + 2;
		}
		else
		{
			out_char[i] = c;
		}

	}
	out = out_char;

	return 0;
}

//TODO: Err handling is poor currently.
//Always returns NULL.
void *child(void* arg) {

	int * tid = (int *) arg;

	msg * m;
	int client;

	while (1) {
		sem_wait(&sem);
		if (pthread_mutex_lock(&stdout_mutex))
		{
			perror("WARN: Failed to lock stdout mutex.\n");
			continue;
		}
		client = queue_pop(q);

		printf("Thread %d is handling client %d\n", *tid, client);

		pthread_mutex_unlock(&stdout_mutex);

		m = malloc(sizeof(msg));
		m->ID = 100;
		m->len = 0;
		if ((send(client, m, sizeof(msg), 0)) == -1)
		{
			perror("WARN: Failed to send handshake message.\n");
			continue;
		}

		if ((recv(client, m, sizeof(msg), 0)) == -1)
		{
			perror("WARN: Failed to recieve handshake response message.\n");
			continue;
		}

		if (m->ID != 101)
		{
			perror("WARN: Client failed to send handshake response.\n");
			continue;
		}

		m->payload = (char *) malloc(sizeof(char) * 161);

		int maxSize = 161 + sizeof(int) * 2;
		int done = 1;

		while(done == 1)
		{
			if ((recv(client, m, maxSize, 0)) == -1)
			{
				perror("WARN: Failed to recieve payload message.\n");
				continue;
			}
			switch (m->ID)
			{
				case 102:
					m->ID = 103;
					char * temp = (char *) malloc(sizeof(char) * m->len);
					if (decrypt(m->payload, temp, m->len) == 2)
					{
						m->payload = "";
					}
					else
					{
						m->payload = temp;
					}
					if ((send(client, m, maxSize, 0)) == -1)
					{
						perror("WARN: Failed to send decrypted text back.\n");
					}
					break;
				case 104:
					done = 0;
					break;
				case 105:
					perror("WARN: Client sent error message.\n");
					done = 0;
					break;
				default:
					perror("WARN: Wrong message type.\n");
					break;
			}

		}

		if (pthread_mutex_lock(&stdout_mutex))
		{
			perror("WARN: Failed to lock stdout mutex.\n");
			continue;
		}


		printf("Thread %d finished handling client %d\n", *tid, client);




		//If fails, don't have any choice!
		pthread_mutex_unlock(&stdout_mutex);


		//just to make sure most get a fair chance
		//of execution
		sleep(1);
	}

	return NULL;
}


//Usage:
//./retrieve client_dir output_dir [num_threads]
int main(int argc, char* argv[]) {

	char *clients = NULL;
	int num_threads = 5, i, error;
	pthread_t *tids;
	int server_port;
	int socketfd;
	int clientfd;
	int clientlen;
	struct sockaddr_in clientaddr;
	struct sockaddr_in serveraddr;
	switch (argc) {
	case 3:
		num_threads = atoi(argv[2]);
		//clears the memory, +1 for NULL
	case 2:
		server_port = atoi(argv[1]);
		break;
	default:
		perror("ERROR: Invalid number of arguments, see usage in README.\n");
		break;
	}
	if ((sem_init(&sem, 0, 0)) < 0)
	{
		perror("ERROR: Failed to initiate semaphore.\n");
		return 1;
	}

	//read clients.txt file

	queue_init(&q);

	if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("ERROR: Failed to open socket.\n");
		return 1;
	}

	bzero((char *) &serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = INADDR_ANY;
	serveraddr.sin_port = htons(server_port);


	//allocate pthreads
	if ((tids = (pthread_t *) calloc(num_threads, sizeof(pthread_t))) == NULL) {
		perror("ERROR: Failed to calloc thread id array.\n");
		return 1;
	}

	if (bind(socketfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0)
	{
		perror("ERROR: Failed to bind.\n");
		return 1;
	}

	if(listen(socketfd, 10) < 0)
	{
		perror("ERROR: Failed to open socket.\n");
		return 1;
	}

	if ((pthread_mutex_init(&stdout_mutex, NULL)) < 0)
	{
		perror("ERROR: Failed to initiate mutex.\n");
		return 1;

	}

	//create threads
	for (i = 0; i < num_threads; ++i)
	{
		int * arg = malloc(sizeof(*arg));
		*arg = i + 1;
		if ((error = pthread_create(tids + i, NULL, child, (void *) arg)))
		{
			perror("WARN: Failed to create a thread.\n");
		}
	}

	//add more clients to the queue
	while(1)
	{
		sem_post(&sem);
		clientlen = (sizeof(clientaddr));
		if ((clientfd = accept(socketfd, (struct sockaddr *) &clientaddr, (socklen_t *) &clientlen)) < 0)
		{
			pthread_mutex_lock(&stdout_mutex);
			perror("WARN: Issue connecting to client.\n");
			pthread_mutex_unlock(&stdout_mutex);
			continue;
		}
		if ((queue_push(q,clientfd)) == 1)
		{
			pthread_mutex_lock(&stdout_mutex);
			perror("WARN: Issue pushing data in queue.\n");
			pthread_mutex_unlock(&stdout_mutex);
		}

	}

	//wait for threads
	for (i = 0; i < num_threads; ++i) {
		if (pthread_join(tids[i], NULL)) {
			perror("WARN");
		}
	}

	//clean up
	clean_up(q);
	free(tids);
	free(clients);
	free(output_dir);

	return 0;
}
