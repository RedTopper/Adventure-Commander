#ifndef ENTITY_H
#define ENTITY_H

#include <wchar.h>

#include "point.h"

extern wchar_t* ENTITY_SYMBOLS[2];

typedef enum {
	STAIRS_UP,
	STAIRS_DOWN
} EntityType;

typedef struct {
	Point pos;
	EntityType type;
	wchar_t* symbol;
} Entity;

#endif

#ifndef ENTITY_FUNCTIONS
#include "dungeon.h"
#define ENTITY_FUNCTIONS

void entityGenerateAll(Dungeon* dungeon, int floor);

#endif
