#pragma once

#include <wchar.h>

#include "point.h"

typedef struct {
	Point pos;
	Point known;
	int skills;
	int speed;
	int order;
} Mob;

extern const wchar_t* SYM_PLAY;