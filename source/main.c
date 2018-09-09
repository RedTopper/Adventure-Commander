#include <wchar.h>
#include <locale.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>

#include "types.h"
#include "dungeon.h"

int main()
{
	setlocale(LC_ALL, "");
	srand(time(NULL));
	Dungeon dungeon = dungeonGenerate(DUNGEON_DIM);
	dungeonPrint(dungeon);
	dungeonDestroy(dungeon);
}