#ifndef DUNGEON_H
#define DUNGEON_H

#include <wchar.h>
#include <inttypes.h>
#include <ncursesw/curses.h>

#include "mob.h"
#include "point.h"
#include "queue.h"

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
	wchar_t* status;
	wchar_t* line1;
	wchar_t* line2;
	Point dim;
	Mob* mobs;
	Mob* player;
	Tile** tiles;
	Room* rooms;
	QueueNode* turn;
	int numRooms;
	int numMobs;
	int emoji;
	int** pathFloor;
	int** pathDig;
} Dungeon;

extern const Point DUNGEON_DIM;

void setText(Dungeon dungeon, wchar_t** buffer, wchar_t* text);
Dungeon dungeonGenerate(Point dim, int mobs, int emoji);
Dungeon dungeonLoad(FILE* file, int mobs, int emoji);
void dungeonSave(Dungeon dungeon, FILE* file);
void dungeonDestroy(Dungeon* dungeon);
void dungeonPrint(WINDOW* window, Dungeon dungeon);

#endif
