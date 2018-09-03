#include <wchar.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "dungeon.h"

//Number of rooms
const int ROOMS_MIN = 5;
const int ROOMS_MAX = 8;
const float ROOM_MAX_FULLNESS = 0.15f;

//Size of rooms. 
const int ROOM_X_MIN = 7;
const int ROOM_X_MAX = 16;
const int ROOM_Y_MIN = 3;
const int ROOM_Y_MAX = 8;

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

//Hall
const wchar_t* SYM_HALL = L"\x2588";
const wchar_t* SYM_HALL_NESW = L"\x256C";
const wchar_t* SYM_HALL_NES = L"\x2560";
const wchar_t* SYM_HALL_ESW = L"\x2566";
const wchar_t* SYM_HALL_SWN = L"\x2563";
const wchar_t* SYM_HALL_WNE = L"\x2569";
const wchar_t* SYM_HALL_NE = L"\x255A";
const wchar_t* SYM_HALL_ES = L"\x2554";
const wchar_t* SYM_HALL_SW = L"\x2557";
const wchar_t* SYM_HALL_WN = L"\x255D";
const wchar_t* SYM_HALL_NS = L"\x2551";
const wchar_t* SYM_HALL_EW = L"\x2550";

//Rooms are air
const wchar_t* SYM_ROOM = L" ";

int skewedBetweenRange(int min, int max) {
	//75% chance of "upgrading" size.
	int value = min;
	while (rand() % 4 && value < max) value++;
	return value;
}

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

int roomPlaceAttempt(Dungeon dungeon, Room room) {
	for (int row = room.pos.y - 1; row < room.pos.y + room.dim.y + 1; row++) {
		for (int col = room.pos.x - 1; col < room.pos.x + room.dim.x + 1; col++) {
			if (dungeon.tiles[row][col].type == ROOM) return 0;
		}
	}
	
	return 1;
}

void roomPlace(Dungeon dungeon, Room room) {
	for (int row = room.pos.y; row < room.pos.y + room.dim.y; row++) {
		for (int col = room.pos.x; col < room.pos.x + room.dim.x; col++) {
			Tile* tile = &dungeon.tiles[row][col];
			tile->type = ROOM;
			tile->symbol = SYM_ROOM;
		}
	}
}

Room roomGenerate(Dungeon dungeon) {
	Point dim = {0};
	Point pos = {0};
	Room room = {0};
	
	do {
		dim = (Point){
			skewedBetweenRange(ROOM_X_MIN, ROOM_X_MAX),
			skewedBetweenRange(ROOM_Y_MIN, ROOM_Y_MAX)
		};
		
		pos = (Point){
			(rand() % (dungeon.dim.x - 1 - dim.x)) + 1,
			(rand() % (dungeon.dim.y - 1 - dim.y)) + 1
		};
		
		room = (Room){pos, dim};
	} while (!roomPlaceAttempt(dungeon, room));

	return room;
}

void roomConnect(Dungeon dungeon, Room first, Room second) {
	Point pos = {
		first.pos.x,
		first.pos.y
	};
	
	Point dir = {
		first.pos.x < second.pos.x ? 1 : -1,
		first.pos.y < second.pos.y ? 1 : -1
	};

	//Horizontal connection
	while (pos.x != second.pos.x) {
		Tile* tile = &dungeon.tiles[pos.y][pos.x];
		if (tile->type == ROCK) {
			tile->type = HALL;
			tile->symbol = SYM_HALL;
		}
		
		pos.x += dir.x;
	}
	
	//Vertical connection
	while (pos.y != second.pos.y) {
		Tile* tile = &dungeon.tiles[pos.y][pos.x];
		if (tile->type == ROCK) {
			tile->type = HALL;
			tile->symbol = SYM_HALL;
		}
		
		pos.y += dir.y;
	}
}

int dungeonIsFull(Dungeon dungeon) {
	float total = dungeon.dim.x * dungeon.dim.y;
	float room = 0.0;
	for(int row = 0; row < dungeon.dim.y; row++) {
		for(int col = 0; col < dungeon.dim.x; col++) {
			if (dungeon.tiles[row][col].type == ROOM) room = room + 1.0;
		}
	}
	
	return room/total > ROOM_MAX_FULLNESS;
}

void dungeonPaths(Dungeon dungeon, Room rooms[], int count) {
	for(int first = 0; first < count; first++) {
		for(int second = first + 1; second < count; second++) {
			roomConnect(dungeon, rooms[first], rooms[second]);
		}
	}
}

void dungeonPostProcess(Dungeon dungeon) {
	for(int row = 1; row < dungeon.dim.y - 1; row++) {
		for(int col = 1; col < dungeon.dim.x - 1; col++) {
			Tile* tile = &dungeon.tiles[row][col];
			Tile* tileN = &dungeon.tiles[row - 1][col];
			Tile* tileE = &dungeon.tiles[row][col + 1];
			Tile* tileS = &dungeon.tiles[row + 1][col];
			Tile* tileW = &dungeon.tiles[row][col - 1];
			if (tile->type == HALL) {
				
				//NESW
				if ((tileN->type == HALL || tileN->type == ROOM)
					&& (tileE->type == HALL || tileE->type == ROOM)
					&& (tileS->type == HALL || tileS->type == ROOM)
					&& (tileW->type == HALL || tileW->type == ROOM)) {
					tile->symbol = SYM_HALL_NESW;
				}
				
				//NES
				else if ((tileN->type == HALL || tileN->type == ROOM)
					&& (tileE->type == HALL || tileE->type == ROOM) 
					&& (tileS->type == HALL || tileS->type == ROOM)) {
					tile->symbol = SYM_HALL_NES;
				}
					
				//ESW
				else if ((tileE->type == HALL || tileE->type == ROOM)
					&& (tileS->type == HALL || tileS->type == ROOM)
					&& (tileW->type == HALL || tileW->type == ROOM)) {
					tile->symbol = SYM_HALL_ESW;
				}	
				
				//SWN
				else if ((tileS->type == HALL || tileS->type == ROOM)
					&& (tileW->type == HALL || tileW->type == ROOM)
					&& (tileN->type == HALL || tileN->type == ROOM)) {
					tile->symbol = SYM_HALL_SWN;
				}
				
				//WNE
				else if ((tileW->type == HALL || tileW->type == ROOM)
					&& (tileN->type == HALL || tileN->type == ROOM)
					&& (tileE->type == HALL || tileE->type == ROOM)) {
					tile->symbol = SYM_HALL_WNE;
				}
				
				//NE
				else if ((tileN->type == HALL || tileN->type == ROOM)
					&& (tileE->type == HALL || tileE->type == ROOM)) {
					tile->symbol = SYM_HALL_NE;
				}
				
				//ES
				else if ((tileE->type == HALL || tileE->type == ROOM) 
					&& (tileS->type == HALL || tileS->type == ROOM)) {
					tile->symbol = SYM_HALL_ES;
				}
				
				//SW
				else if ((tileS->type == HALL || tileS->type == ROOM)
					&& (tileW->type == HALL || tileW->type == ROOM)) {
					tile->symbol = SYM_HALL_SW;
				}
				
				//WN
				else if ((tileW->type == HALL || tileW->type == ROOM)
					&& (tileN->type == HALL || tileN->type == ROOM)) {
					tile->symbol = SYM_HALL_WN;
				}
				
				//NS
				else if ((tileN->type == HALL || tileN->type == ROOM)
					&& (tileS->type == HALL || tileS->type == ROOM)) {
					tile->symbol = SYM_HALL_NS;
				}
				
				//EW
				else if ((tileE->type == HALL || tileE->type == ROOM)
					&& (tileW->type == HALL || tileW->type == ROOM)) {
					tile->symbol = SYM_HALL_EW;
				}
			}
		}
	}
}

//"Public" functions

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
	
	Room rooms[ROOMS_MAX];
	int room = 0;
	
	//Create rooms
	for(room = 0; room < ROOMS_MAX; room++) {
		if (room >= ROOMS_MIN && dungeonIsFull(dungeon)) break;
		rooms[room] = roomGenerate(dungeon);
		roomPlace(dungeon, rooms[room]);
	}
	
	//Create the paths.
	dungeonPaths(dungeon, rooms, room);
	
	//Prettify the dungeon.
	dungeonPostProcess(dungeon);
	
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