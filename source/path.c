#include <stdlib.h>

#include "types.h"
#include "queue.h"
#include "path.h"

int** pathCreateStyle(Dungeon dungeon, PathFinderStyle style) {
	//Destroy previous path, if it exists.
	int** path = malloc(sizeof(int*) * dungeon.dim.y);
	for (int row = 0; row < dungeon.dim.y; row++) {
		path[row] = malloc(sizeof(int) * dungeon.dim.x);
	}

	Node* queue = queueCreate(dungeon.player, 1);

	while (!queueEmpty(&queue)) {
		Point p = queuePeek(&queue);

	}

	return path;
}

void pathFree(Dungeon dungeon, int** path) {
	//Can't free if there's nothing to free!
	if (!path) return;
	for (int row = 0; row < dungeon.dim.y; row++) {
		free(path[row]);
	}

	free(path);
}

//"Public" functions

void pathCreate(Dungeon* dungeon) {
	pathDestroy(dungeon);
	dungeon->pathFloor = pathCreateStyle(*dungeon, PATH_VIA_FLOOR);
	dungeon->pathDig = pathCreateStyle(*dungeon, PATH_VIA_WALLS);
}

void pathDestroy(Dungeon* dungeon) {
	pathFree(*dungeon, dungeon->pathDig);
	pathFree(*dungeon, dungeon->pathFloor);
	dungeon->pathFloor = NULL;
	dungeon->pathDig = NULL;
}
