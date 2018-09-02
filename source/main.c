#include <wchar.h>
#include <locale.h>

#include "types.h"
#include "dungeon.h"

int main()
{
	setlocale(LC_ALL, "");
	Dungeon dungeon = dungeonGenerate(DUNGEON_DIM);
	dungeonPrint(dungeon);
	dungeonDestroy(dungeon);
}