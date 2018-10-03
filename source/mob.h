#ifndef MOB_H
#define MOB_H

#include <wchar.h>

#include "point.h"

typedef struct {
	Point pos;
	Point known;
	int skills;
	int speed;
	int order;
	int hp;
} Mob;

typedef enum {
	INTELLIGENCE = 0x01,
	TELEPATHY = 0x02,
	TUNNELING = 0x04,
	ERRATIC = 0x08,
	PC = 0x10,
} Skills;

extern const wchar_t SYM_PLAY;
extern const wchar_t MOB_TYPES[];

#endif

#ifndef DUNGEON_H

#include "dungeon.h"

Mob* mobGenerateAll(Dungeon dungeon);
Mob mobGeneratePlayer(Point param);

#endif
