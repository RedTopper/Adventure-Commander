#pragma once

Dungeon dungeonGenerate(Point dim);
Dungeon dungeonLoad(FILE* file);
void dungeonSave(Dungeon dungeon, FILE* file);
void dungeonDestroy(Dungeon dungeon);
void dungeonPrint(Dungeon dungeon);
void dungeonPrintDebug(Dungeon dungeon);
