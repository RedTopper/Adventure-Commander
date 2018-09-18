#include <stdlib.h>

#include "types.h"
#include "queue.h"
#include "path.h"

void pathCreate(Dungeon* dungeon) {

	//Destroy previous path, if it exists.
	if (dungeon->path) pathDestroy(dungeon);

	dungeon->path = malloc(sizeof(int) * dungeon->dim.y);

	for (int row = 0; row < dungeon->dim.y; row++) {
		dungeon->path[row] = malloc(sizeof(int) * dungeon->dim.x);
	}

	Node* queue = queueCreate(dungeon->player, 1);
}

void pathDestroy(Dungeon* dungeon) {
	if (!dungeon->path) return;

	for (int row = 0; row < dungeon->dim.y; row++) {
		free(dungeon->path[row]);
	}

	free(dungeon->path);
}
