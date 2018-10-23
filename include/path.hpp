#ifndef PATH_H
#define PATH_H

#include "dungeon.h"

typedef enum {
	PATH_VIA_DIG,
	PATH_VIA_FLOOR
} PathFinderStyle;

extern const int HARDNESS_RATE;
extern const Point ADJACENT[8];

void pathCreate(Dungeon* dungeon);
void pathDestroy(Dungeon* dungeon);

#endif
