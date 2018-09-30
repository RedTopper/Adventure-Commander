#pragma once

#include <wchar.h>
#include <inttypes.h>

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
	Point player;
	Point dim;
	int numRooms;
	Tile** tiles;
	Room* rooms;
	int** pathFloor;
	int** pathDig;
} Dungeon;

extern const Point DUNGEON_DIM;

Dungeon dungeonGenerate(Point dim);
Dungeon dungeonLoad(FILE* file);
void dungeonSave(Dungeon dungeon, FILE* file);
void dungeonDestroy(Dungeon* dungeon);
void dungeonPrint(Dungeon dungeon);
