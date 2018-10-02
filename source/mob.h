#ifndef MOB_H
#define MOB_H

#include <wchar.h>

#include "point.h"

typedef struct mob {
	Point pos;
	Point known;
	int skills;
	int speed;
	int order;
	int hp;
} Mob;

extern const wchar_t* SYM_PLAY;
extern const wchar_t* MOB_TYPES[];

#endif

#ifndef DUNGEON_H

#include "dungeon.h"

Mob* mobGenerateAll(Dungeon dungeon);

#endif

