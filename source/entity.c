#include <stdlib.h>

#include "entity.h"
#include "dungeon.h"

wchar_t* ENTITY_SYMBOLS[2] = {
	L"\x25b2", //Up pointing triangle
	L"\x25bc", //Down pointing triangle
};

static Point entityValidSpawnPoint(Dungeon dungeon) {
	Point pos = {0};

	int onEntity = 1;
	while(onEntity) {
		pos.x = rand() % dungeon.dim.x;
		pos.y = rand() % dungeon.dim.y;
		Tile tile = dungeon.tiles[pos.y][pos.x];

		//Spawn on floor
		if (!(tile.type == ROOM)) continue;

		//Not on player
		if (pos.x == dungeon.player->pos.x || pos.y == dungeon.player->pos.y) continue;

		//Not on another entity
		onEntity = 0;
		for (int entity = 0; !onEntity && entity < dungeon.numEntities; entity++) {
			Entity e = dungeon.entities[entity];
			if (pos.x == e.pos.x && pos.y == e.pos.y) onEntity = 1;
		}
	}

	return pos;
}

const wchar_t* entityGetSymbol(Entity* entity) {
	uint32_t size = sizeof(ENTITY_SYMBOLS)/sizeof(ENTITY_SYMBOLS[0]);
	return ENTITY_SYMBOLS[entity->type >= size ? size - 1 : entity->type];
}

void entityGenerateAll(Dungeon* dungeon, int floor) {
	dungeon->numEntities = 0;
	dungeon->entities = malloc(sizeof(Entity));

	//Create up stairs always
	Entity up = {0};
	up.pos = entityValidSpawnPoint(*dungeon);
	up.type = STAIRS_UP;
	dungeon->entities[dungeon->numEntities] = up;
	dungeon->numEntities++;

	if (floor) {
		//If on a non zero floor, create a down staircase too
		dungeon->entities = realloc(dungeon->entities, sizeof(Entity) * (dungeon->numEntities + 1));

		Entity down = {0};
		down.pos = dungeon->player->pos;
		down.type = STAIRS_DOWN;
		dungeon->entities[dungeon->numEntities] = down;
		dungeon->numEntities++;
	}
}