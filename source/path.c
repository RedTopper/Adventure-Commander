#include <stdlib.h>
#include <stdint.h>

#include "mob.h"
#include "queue.h"
#include "path.h"

//Loop of next points
const Point NEXT[] = {
	{0, -1},
	{1, -1},
	{1,  0},
	{1,  1},
	{0,  1},
	{-1, 1},
	{-1, 0},
	{-1, -1}
};

int** pathCreateStyle(Dungeon dungeon, PathFinderStyle style) {

	int** distance = malloc(sizeof(int*) * dungeon.dim.y);
	for (int row = 0; row < dungeon.dim.y; row++) {
		distance[row] = malloc(sizeof(int) * dungeon.dim.x);
		for (int col = 0; col < dungeon.dim.x; col++) {
			distance[row][col] = INT32_MAX;
		}
	}

	//Create node for player and set distance to player to zero.
	Node* queue = queueCreate((NodeData){.pos=dungeon.player.pos}, 0);
	distance[dungeon.player.pos.y][dungeon.player.pos.x] = 0;

	while (!queueEmpty(&queue)) {
		Point current = queuePeek(&queue).pos;
		queuePop(&queue);

		int distCurrent = distance[current.y][current.x];
		for(int i = 0; i < (int)(sizeof(NEXT)/sizeof(NEXT[0])); i++) {
			Point next = {0};
			next.x = current.x + NEXT[i].x;
			next.y = current.y + NEXT[i].y;

			//Make sure nothing bad happens
			if (next.x < 0 || next.y < 0 || next.x >= dungeon.dim.x || next.y >= dungeon.dim.y) continue;

			//Filter out types depending on mode.
			TileType type = dungeon.tiles[next.y][next.x].type;
			if (type == VOID || type == EDGE) continue;
			if (style == PATH_VIA_FLOOR && type == ROCK) continue;

			int currentHardness = dungeon.tiles[current.y][current.x].hardness;
			int distNextOld = distance[next.y][next.x];
			int distNextNew = distCurrent + 1 + (currentHardness / 85);

			if (distNextOld > distNextNew) {
				distance[next.y][next.x] = distNextNew;
				queuePush(&queue, (NodeData){.pos=next}, distNextNew);
			}
		}
	}

	return distance;
}

void pathFree(Dungeon dungeon, int** path) {
	//Can't free if there's nothing to free!
	if (path == NULL) return;
	for (int row = 0; row < dungeon.dim.y; row++) {
		free(path[row]);
	}

	free(path);
}

//"Public" functions

void pathCreate(Dungeon* dungeon) {
	pathDestroy(dungeon);
	dungeon->pathDig = pathCreateStyle(*dungeon, PATH_VIA_DIG);
	dungeon->pathFloor = pathCreateStyle(*dungeon, PATH_VIA_FLOOR);
}

void pathDestroy(Dungeon* dungeon) {
	pathFree(*dungeon, dungeon->pathDig);
	pathFree(*dungeon, dungeon->pathFloor);
	dungeon->pathDig = NULL;
	dungeon->pathFloor = NULL;
}

void pathPrint(Dungeon dungeon, int** path) {
	for (int row = 0; row < dungeon.dim.y; row++) {
		for (int col = 0; col < dungeon.dim.x; col++) {
			if (row == dungeon.player.pos.y && col == dungeon.player.pos.x) {
				wprintf(SYM_PLAY);
			} else if (path[row][col] != INT32_MAX) {
				wprintf(L"%d", path[row][col] % 10);
			} else {
				wprintf(L" ");
			}
		}
		wprintf(L"\n");
	}
}
