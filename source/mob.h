#ifndef MOB_H
#define MOB_H

#include <wchar.h>
#include <ncursesw/curses.h>

#include "point.h"

typedef struct {
	Point pos;
	int known;
	int skills;
	int speed;
	int order;
	int hp;
} Mob;

typedef enum {
	MOVE_FAILURE,
	MOVE_SUCCESS,
	MOVE_BROKE_WALL,
	MOVE_DAMAGE_WALL,
} Movement;

typedef enum {
	ACTION_NONE,
	ACTION_UP,
	ACTION_DOWN,
	ACTION_MOVE,
	ACTION_QUIT
} Action;

typedef enum {
	SKILL_INTELLIGENCE = 0x01,
	SKILL_TELEPATHY = 0x02,
	SKILL_TUNNELING = 0x04,
	SKILL_ERRATIC = 0x08,
	SKILL_PC = 0x10,
} Skills;

extern const wchar_t* MOB_TYPES[];
extern const wchar_t* MOB_TYPES_BORING[];

#endif

#if !(defined(DUNGEON_H) || defined(QUEUE_H))
#include "dungeon.h"
#include "queue.h"

const wchar_t* mobGetSymbol(Mob *mob, Dungeon dungeon);
Mob* mobGeneratePlayer(Point point);
Mob* mobGenerateAll(Dungeon dungeon);
void mobCreateQueue(Dungeon* dungeon);
int mobAliveCount(Dungeon dungeon);
Action mobTick(Mob* mob, Dungeon* dungeon, WINDOW* base);

#endif

