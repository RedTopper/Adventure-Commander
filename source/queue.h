#ifndef QUEUE_H
#define QUEUE_H

#include "mob.h"
#include "point.h"

/**
 * A priority queue for C, altered to use unions instead of ints.
 *
 * higher values indicate higher priority
 *
 * Author: https://www.geeksforgeeks.org/priority-queue-using-linked-list/
 */

typedef union {
	Point pos;
	Mob* mob;
} NodeData;

typedef struct node {
	NodeData data;
	int priority;
	int prioritySub;
	struct node* next;
} Node;

Node* queueCreate(NodeData data, int priority);
NodeData queuePeek(Node** head);
void queuePush(Node** head, NodeData data, int priority);
void queuePop(Node** head);
int queueEmpty(Node** head);
int queuePeekPriority(Node** head);

#endif
