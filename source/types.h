#pragma once

typedef enum {
	VOID,
	EDGE,
	ROCK,
	ROOM,
	HALL,
} TileType;

typedef struct {
	int x;
	int y;
} Point;

typedef struct {
	const wchar_t* symbol;
	TileType type;
} Tile;

typedef struct {
	Tile** tiles;
	Point dim;
} Dungeon;

extern const Point DUNGEON_DIM;
