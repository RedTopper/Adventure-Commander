#ifndef STACK_H
#define STACK_H

#include "dungeon.h"

typedef union {
	Dungeon dungeon;
} StackNodeData;

typedef struct queueNode {
	StackNodeData data;
	struct queueNode* next;
} StackNode;

StackNode* stackCreate(StackNodeData data);
StackNodeData stackPeek(StackNode** head);
void stackPush(StackNode** head, StackNodeData data);
void stackPop(StackNode** head);
int stackEmpty(StackNode** head);
void stackDestroy(StackNode** head);

#endif
