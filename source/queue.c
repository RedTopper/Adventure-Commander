#include <stdio.h>
#include <stdlib.h>

#include "point.h"
#include "queue.h"

// Function to Create A New Node
// Also used to create new queues externally.
Node* queueCreate(NodeData data, int priority) {
	Node* temp = (Node*) malloc(sizeof(Node));
	temp->data = data;
	temp->priority = priority;
	temp->next = NULL;
	return temp;
}

NodeData queuePeek(Node** head) {
	return (*head)->data;
}

void queuePop(Node** head) {
	Node* temp = *head;
	(*head) = (*head)->next;
	free(temp);
}

void queuePush(Node** head, NodeData data, int priority) {
	Node* start = (*head);
	Node* temp = queueCreate(data, priority);

	if ((*head) == NULL || priority < (*head)->priority) {
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