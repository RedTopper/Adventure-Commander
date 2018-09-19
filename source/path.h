#pragma once

#include "types.h"

void pathCreate(Dungeon* dungeon);
void pathDestroy(Dungeon* dungeon);
void pathPrint(Dungeon dungeon, int** path);