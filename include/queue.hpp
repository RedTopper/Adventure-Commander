#ifndef QUEUE_H
#define QUEUE_H

#include "mob.h"
#include "point.h"

/**
 * A priority queue for C, altered to use unions instead of ints.
 *
 * lower values indicate higher priority
 *
 * Author: https://www.geeksforgeeks.org/priority-queue-using-linked-list/
 */

typedef union {
	Point pos;
	Mob* mob;
} QueueNodeData;

typedef struct node {
	QueueNodeData data;
	int priority;
	int prioritySub;
	struct node* next;
} QueueNode;

QueueNode* queueCreate(QueueNodeData data, int priority);
QueueNode* queueCreateSub(QueueNodeData data, int priority, int prioritySub);
QueueNodeData queuePeek(QueueNode** head);
int queuePeekPriority(QueueNode** head);
void queuePush(QueueNode** head, QueueNodeData data, int priority);
void queuePushSub(QueueNode** head, QueueNodeData data, int priority, int prioritySub);
void queuePop(QueueNode** head);
int queueEmpty(QueueNode** head);
void queueDestroy(QueueNode** head);

#endif
