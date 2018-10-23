#include <stdio.h>
#include <stdlib.h>

#include "mob.h"
#include "point.h"
#include "queue.h"

//returns 1 if the first two parameters have a higher priority than the comparing node.
static int compare(int priority, int prioritySub, QueueNode node) {
	return (priority < node.priority || (priority == node.priority && prioritySub < node.prioritySub));
}

//"Public" functions

// Function to Create A New QueueNode
// Also used to create new queues externally.
QueueNode* queueCreateSub(QueueNodeData data, int priority, int prioritySub) {
	QueueNode* temp = (QueueNode*) malloc(sizeof(QueueNode));
	temp->data = data;
	temp->priority = priority;
	temp->prioritySub = prioritySub;
	temp->next = NULL;
	return temp;
}

QueueNode* queueCreate(QueueNodeData data, int priority) {
	return queueCreateSub(data, priority, 0);
}

QueueNodeData queuePeek(QueueNode** head) {
	return (*head)->data;
}

int queuePeekPriority(QueueNode** head) {
	return (*head)->priority;
}

void queuePop(QueueNode** head) {
	QueueNode* temp = *head;
	(*head) = (*head)->next;
	free(temp);
}

void queuePushSub(QueueNode** head, QueueNodeData data, int priority, int prioritySub) {
	QueueNode* start = (*head);
	QueueNode* temp = queueCreateSub(data, priority, prioritySub);

	if ((*head) == NULL || compare(priority, prioritySub, (**head))) {
		//If the priority is higher than the head,
		//replace the head node.
		temp->next = *head;
		(*head) = temp;
	} else {
		//Otherwise, traverse until we can insert a node.
		while (start->next != NULL && !compare(priority, prioritySub, *start->next)) {
			start = start->next;
		}

		temp->next = start->next;
		start->next = temp;
	}
}

void queuePush(QueueNode** head, QueueNodeData data, int priority) {
	queuePushSub(head, data, priority, 0);
}

int queueEmpty(QueueNode** head) {
	return (*head) == NULL;
}

void queueDestroy(QueueNode** head) {
	while (!queueEmpty(head)) {
		queuePop(head);
	}
}