#ifndef DUNGEON_H
#define DUNGEON_H

//Unsafe to use functions
//Dungeon not defined yet
#define ENTITY_FUNCTIONS
#define MOB_FUNCTIONS

#include <wchar.h>
#include <inttypes.h>
#include <ncursesw/curses.h>

#include "mob.h"
#include "entity.h"
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
	Mob* player;
	Mob* mobs;
	Room* rooms;
	Entity* entities;
	QueueNode* turn;
	Tile** tiles;
	int emoji;
	int numMobs;
	int numRooms;
	int numEntities;
	int** pathFloor;
	int** pathDig;
} Dungeon;

//Now safe to use functions
#undef ENTITY_FUNCTIONS
#undef MOB_FUNCTIONS

extern const Point DUNGEON_DIM;

void setText(Dungeon dungeon, wchar_t** buffer, wchar_t* text);
Dungeon dungeonGenerate(Point dim, int mobs, int emoji, int floor);
Dungeon dungeonLoad(FILE* file, int mobs, int emoji, int floor);
void dungeonSave(Dungeon dungeon, FILE* file);
void dungeonDestroy(Dungeon* dungeon);
void dungeonPrint(WINDOW* window, Dungeon dungeon);

#endif
