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

#if !(defined(DUNGEON_H) || defined(QUEUE_H))
#include "dungeon.h"
#include "queue.h"

Mob mobGeneratePlayer(Point point);
Mob* mobGenerateAll(Dungeon dungeon);
Node* mobCreateQueue(Dungeon dungeon);
int mobAliveCount(Dungeon dungeon);

#endif

