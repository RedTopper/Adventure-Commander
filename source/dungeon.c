#include <wchar.h>
#include <stdlib.h>
#include <stdio.h>

#include "types.h"
#include "dungeon.h"

//In case I ever want different syms for each corner
const wchar_t* SYM_EDGE_NW = L" ";
const wchar_t* SYM_EDGE_NE = L" ";
const wchar_t* SYM_EDGE_SE = L" ";
const wchar_t* SYM_EDGE_SW = L" ";

//Edges across board
const wchar_t* SYM_EDGE_N = L"\x2584";
const wchar_t* SYM_EDGE_E = L"\x2588";
const wchar_t* SYM_EDGE_S = L"\x2580";
const wchar_t* SYM_EDGE_W = L"\x2588";

//Rocks
const wchar_t* SYM_ROCK_SOFT = L"\x2591";
const wchar_t* SYM_ROCK_MED  = L"\x2592";
const wchar_t* SYM_ROCK_HARD = L"\x2593";

//Rooms are air
const wchar_t* SYM_ROOM = L" ";

void drawNorthSouth(Dungeon dungeon, const wchar_t* sym, int row) {
	for(int col = 0; col < dungeon.dim.x; col++) {
		dungeon.tiles[row][col] = (Tile){
			sym,
			EDGE
		};
	}
}

void drawEastWest(Dungeon dungeon, const wchar_t* sym, int col) {
	for(int row = 0; row < dungeon.dim.y; row++) {
		dungeon.tiles[row][col] = (Tile){
			sym,
			EDGE
		};
	}
}

Dungeon dungeonGenerate(Point dim) {
	Dungeon dungeon = {0};
	dungeon.dim = dim;
	
	if (dim.x < 1 || dim.y < 1) return dungeon;
	
	//Init dungeon
	dungeon.tiles = (Tile**) malloc(sizeof(Tile*) * dim.y);
	for(int row = 0; row < dim.y; row++) {
		dungeon.tiles[row] = malloc(sizeof(Tile) * dim.x);
		for(int col = 0; col < dim.x; col++) {
			dungeon.tiles[row][col] = (Tile){
				SYM_ROCK_SOFT,
				ROCK
			};
		}
	}
	
	//Draw walls wall
	drawNorthSouth(dungeon, SYM_EDGE_N, 0);
	drawEastWest(dungeon, SYM_EDGE_E, dim.x - 1);
	drawNorthSouth(dungeon, SYM_EDGE_S, dim.y - 1);
	drawEastWest(dungeon, SYM_EDGE_W, 0);

	
	//Draw corners
	dungeon.tiles[0][0].symbol = SYM_EDGE_NW;
	dungeon.tiles[0][dim.x - 1].symbol = SYM_EDGE_NE;
	dungeon.tiles[dim.y - 1][dim.x - 1].symbol = SYM_EDGE_SE;
	dungeon.tiles[dim.y - 1][0].symbol = SYM_EDGE_SW;
	
	//Create rooms
	
	return dungeon;
}

void dungeonDestroy(Dungeon dungeon) {
	for(int row = 0; row < dungeon.dim.y; row++) {
		free(dungeon.tiles[row]);
	}
	
	free(dungeon.tiles);
}

void dungeonPrint(Dungeon dungeon) {
	for(int row = 0; row < dungeon.dim.y; row++) {
		for(int col = 0; col < dungeon.dim.x; col++) {
			wprintf(L"%ls", dungeon.tiles[row][col].symbol);
		}
		wprintf(L"\n");
	}
}