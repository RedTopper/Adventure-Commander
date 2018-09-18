#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "queue.h"

// Function to Create A New Node
// Also used to create new queues externally.
Node* queueCreate(Point pos, int priority) {
	Node* temp = (Node*) malloc(sizeof(Node));
	temp->pos = pos;
	temp->priority = priority;
	temp->next = NULL;
	return temp;
}

Point queuePeek(Node** head) {
	return (*head)->pos;
}

void queuePop(Node** head) {
	Node* temp = *head;
	(*head) = (*head)->next;
	free(temp);
}

void queuePush(Node** head, Point pos, int priority) {
	Node* start = (*head);
	Node* temp = queueCreate(pos, priority);


	if (priority < (*head)->priority) {
		//If the priority is higher than the head,
		//replace the head node.
		temp->next = *head;
		(*head) = temp;
	} else {
		//Otherwise, traverse until we can insert a node.
		while (start->next != NULL && priority > start->next->priority) {
			start = start->next;
		}

		temp->next = start->next;
		start->next = temp;
	}
}

int queueEmpty(Node** head) {
	return (*head) == NULL;
}