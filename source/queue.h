#pragma once

#include "point.h"

/**
 * A priority queue for C, altered to use points instead of ints (though it really
 * should use void* but that would be too easy, maybe some day).
 *
 * Author: https://www.geeksforgeeks.org/priority-queue-using-linked-list/
 */

// higher values indicate higher priority
typedef struct node {
	Point pos;
	int priority;
	struct node* next;
} Node;

Node* queueCreate(Point pos, int priority);
void queuePush(Node** head, Point pos, int priority);
Point queuePeek(Node** head);
void queuePop(Node** head);
int queueEmpty(Node** head);