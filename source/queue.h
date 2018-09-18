#pragma once

/**
 * A priority queue for C, altered to use void pointers instead of
 * integers.
 *
 * Author: https://www.geeksforgeeks.org/priority-queue-using-linked-list/
 */

// higher values indicate higher priority
typedef struct node {
	void* data;
	int priority;
	struct node* next;
} Node;

Node* queueCreate(void* data, int priority);
void queuePush(Node** head, void* data, int priority);
void* queuePeek(Node** head);
void queuePop(Node** head);
int queueEmpty(Node** head);