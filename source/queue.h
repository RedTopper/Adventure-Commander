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
Node* queueCreateSub(NodeData data, int priority, int prioritySub);
NodeData queuePeek(Node** head);
int queuePeekPriority(Node** head);
void queuePush(Node** head, NodeData data, int priority);
void queuePushSub(Node** head, NodeData data, int priority, int prioritySub);
void queuePop(Node** head);
int queueEmpty(Node** head);
void queueDestroy(Node** head);

#endif
