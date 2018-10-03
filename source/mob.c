#include <wchar.h>
#include <stdlib.h>

#include "mob.h"
#include "dungeon.h"

const wchar_t SYM_PLAY = L'@';
const wchar_t MOB_TYPES[] = {
	L'0',
	L'1',
	L'2',
	L'3',
	L'4',
	L'5',
	L'6',
	L'7',
	L'8',
	L'9',
	L'a',
	L'b',
	L'c',
	L'd',
	L'e',
	L'f',
};

static Point mobValidSpawnPoint(Dungeon dungeon, Mob* mobs, int i) {
	Point pos = {0};

	while(1) {
		pos.x = rand() % dungeon.dim.x;
		pos.y = rand() % dungeon.dim.y;
		Tile tile = dungeon.tiles[pos.y][pos.x];

		//Spawn on floor
		if (!(tile.type == ROOM || tile.type == HALL)) continue;

		//Not on player
		if (pos.x == dungeon.player.pos.x || pos.y == dungeon.player.pos.y) continue;

		//Not on mob
		int exists = 0;
		for (int mob = 0; !exists && mob < i; mob++) {
			Mob m = mobs[mob];
			if (pos.x == m.pos.x && pos.y == m.pos.y) exists = 1;
		}

		if (!exists) break;
	}

	return pos;
}

Mob mobGeneratePlayer(Point point) {
	Mob mob = {0};
	mob.pos = point;
	mob.known = (Point){0};
	mob.skills = PC;
	mob.speed = 10;
	mob.order = 1;
	mob.hp = 1;
	return mob;
}

Mob* mobGenerateAll(Dungeon dungeon) {
	Mob* mobs = malloc(sizeof(Mob) * dungeon.numMobs);
	for (int i = 0; i < dungeon.numMobs; i++) {
		Mob mob = {0};
		mob.pos = mobValidSpawnPoint(dungeon, mobs, i);
		mob.known = (Point){0};
		mob.skills = rand() % 16;
		mob.speed = (rand() % 16) + 5; //5-20
		mob.order = i + 1;
		mob.hp = 1;
		mobs[i] = mob;
	}

	return mobs;
}

