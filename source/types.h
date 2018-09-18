#pragma once

#include "wchar.h"
#include "stdint.h"

typedef enum {
	VOID,
	EDGE,
	ROCK,
	ROOM,
	HALL,
} TileType;

typedef enum {
	FILE_READ_BAD = 0x10,
	FILE_READ_BAD_HEAD,
	FILE_READ_BAD_VERSION,
	FILE_READ_GONE,
	FILE_READ_EOF = 0x20,
	FILE_READ_EOF_SIZE,
	FILE_READ_EOF_PLAYER,
	FILE_READ_EOF_HARDNESS,
	FILE_READ_EOF_ROOMS,
	ARGUMENT_BAD = 0x30,
	ARGUMENT_NO_DASH,
	ARGUMENT_NO_UNKNOWN,
} Error;

typedef struct {
	int x;
	int y;
} Point;

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
	int** path;
} Dungeon;

extern const Point DUNGEON_DIM;
