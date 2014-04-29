/*
 * queue
 *
 *      Author: Abhijeet
 */

#include "main.h"
#include "queue.h"

static pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;

void queue_init(queue_t **q) {

	*q = (queue_t *) calloc(1, sizeof(queue_t));
	if (NULL == *q) {
		perror("ERROR");
		return;
	}

	(*q)->head = NULL;
	(*q)->tail = NULL;
	(*q)->size = -1;
}

//1: error, 0: successful
int queue_push(queue_t *q, int client) {

	queue_node_t *temp;

	if (pthread_mutex_lock(&queue_mutex)) {
		return 1;
	}

	temp = (queue_node_t *) calloc(1, sizeof(queue_node_t));

	temp->client = client;

	temp->next = NULL;

	if (NULL == q->head) {
		q->head = q->tail = temp;
	} else {
		q->tail->next = temp;
		q->tail = temp;
	}

	pthread_mutex_unlock(&queue_mutex);
	return 0;
}

int queue_pop(queue_t *q) {

	queue_node_t *temp;
	int ret_client;

	pthread_mutex_lock(&queue_mutex);

	if (NULL == q->head) {
		perror("ERROR");
		pthread_mutex_unlock(&queue_mutex);
		return -1;
	}

	temp = q->head;
	ret_client = temp->client;


	q->head = q->head->next;
	free(temp);

	pthread_mutex_unlock(&queue_mutex);
	return ret_client;
}

void clean_up(queue_t *q) {
	while (queue_pop(q));
	free(q);
}
