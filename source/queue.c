#include <stdio.h>
#include <stdlib.h>

#include "mob.h"
#include "point.h"
#include "queue.h"

//returns 1 if the first two parameters have a higher priority than the comparing node.
static int compare(int priority, int prioritySub, Node node) {
	return (priority < node.priority || (priority == node.priority && prioritySub < node.prioritySub));
}

//"Public" functions

// Function to Create A New Node
// Also used to create new queues externally.
Node* queueCreateSub(NodeData data, int priority, int prioritySub) {
	Node* temp = (Node*) malloc(sizeof(Node));
	temp->data = data;
	temp->priority = priority;
	temp->prioritySub = prioritySub;
	temp->next = NULL;
	return temp;
}

Node* queueCreate(NodeData data, int priority) {
	return queueCreateSub(data, priority, 0);
}

NodeData queuePeek(Node** head) {
	return (*head)->data;
}

int queuePeekPriority(Node** head) {
	return (*head)->priority;
}

void queuePop(Node** head) {
	Node* temp = *head;
	(*head) = (*head)->next;
	free(temp);
}

void queuePushSub(Node** head, NodeData data, int priority, int prioritySub) {
	Node* start = (*head);
	Node* temp = queueCreateSub(data, priority, prioritySub);

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

void queuePush(Node** head, NodeData data, int priority) {
	queuePushSub(head, data, priority, 0);
}

int queueEmpty(Node** head) {
	return (*head) == NULL;
}

void queueDestroy(Node** head) {
	while (!queueEmpty(head)) {
		queuePop(head);
	}
}