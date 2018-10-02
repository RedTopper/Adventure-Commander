#ifndef PATH_H
#define PATH_H

#include "dungeon.h"

typedef enum {
	PATH_VIA_DIG,
	PATH_VIA_FLOOR
} PathFinderStyle;

void pathCreate(Dungeon* dungeon);
void pathDestroy(Dungeon* dungeon);
void pathPrint(Dungeon dungeon, int** path);

#endif
