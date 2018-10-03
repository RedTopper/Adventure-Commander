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
	wchar_t symbol;
	TileType type;
	uint8_t hardness;
} Tile;

typedef struct {
	Point pos;
	Point dim;
} Room;

typedef struct {
	wchar_t* line1;
	wchar_t* line2;
	wchar_t* prompt;
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

void dungeonPostProcess(Dungeon dungeon);
Dungeon dungeonGenerate(Point dim, int mobs);
Dungeon dungeonLoad(FILE* file, int mobs);
void dungeonSave(Dungeon dungeon, FILE* file);
void dungeonDestroy(Dungeon* dungeon);
void dungeonPrint(Dungeon dungeon);

#endif
