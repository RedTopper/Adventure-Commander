#include <stdlib.h>
#include "stack.h"

StackNode* stackCreate(StackNodeData data) {
	StackNode* temp = (StackNode*) malloc(sizeof(StackNode));
	temp->data = data;
	temp->next = NULL;
	return temp;
}

StackNodeData stackPeek(StackNode** head) {
	return (*head)->data;
}

void stackPush(StackNode** head, StackNodeData data) {
	StackNode* new = stackCreate(data);
	new->next = *head;
	(*head) = new;
}

void stackPop(StackNode** head) {
	StackNode* temp = *head;
	(*head) = (*head)->next;
	free(temp);
}

int stackEmpty(StackNode** head) {
	return (*head) == NULL;
}

void stackDestroy(StackNode** head) {
	while (!stackEmpty(head)) {
		stackPop(head);
	}
}