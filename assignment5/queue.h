/*
 * queue.h
 *
 *  Created on: Apr 5, 2014
 *      Author: Abhijeet
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#define ERR_Q_FULL 2

typedef struct queue_node {
	int client;
	struct queue_node *next;
} queue_node_t;

typedef struct queue {
	queue_node_t *head;
	queue_node_t *tail;

	//these sizes are bounded by Maximum an Int can contain. This is assumed.
	int size; //size of queue
	int max_size; //MAX queue size if bounded.
} queue_t;

void queue_init(queue_t **q);

int queue_push(queue_t *q, int client);

int queue_pop(queue_t *q);


void clean_up(queue_t *q);

#endif /* QUEUE_H_ */
