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
			dungeon.tiles[row][col].type = ROOM;
			dungeon.tiles[row][col].symbol = SYM_ROOM;
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