#ifndef DUNGEON_H
#define DUNGEON_H

#include <wchar.h>
#include <inttypes.h>

#include "mob.h"
#include "point.h"

typedef enum {
	VOID,
	EDGE,
	ROCK,
	ROOM,
	HALL,
} TileType;

typedef struct {
	const wchar_t* symbol;
	TileType type;
	uint8_t hardness;
} Tile;

typedef struct {
	Point pos;
	Point dim;
} Room;

typedef struct {
	Point dim;
	Mob* mobs;
	Mob player;
	Tile** tiles;
	Room* rooms;
	int numRooms;
	int numMobs;
	int** pathFloor;
	int** pathDig;
} Dungeon;

extern const Point DUNGEON_DIM;

Dungeon dungeonGenerate(Point dim);
Dungeon dungeonLoad(FILE* file, int mobs);
void dungeonSave(Dungeon dungeon, FILE* file);
void dungeonDestroy(Dungeon* dungeon);
void dungeonPrint(Dungeon dungeon);

#endif
