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

//Randomness of midpoint, radius
const int ROOM_CON_RAD = 3;
const int ROOM_CON_RAD_MIN = 1;

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
const wchar_t* SYM_HALL = L"\x2022";
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

int prodDot(Point one, Point two) {
	two = (Point){
		two.x - one.x,
		two.y - one.y
	};
	
	return (one.x * two.x) + (one.y * two.y); 
}

int prodCross(Point one, Point two) {
	two = (Point){
		two.x - one.x,
		two.y - one.y
	};
	
	return (one.x * two.y) - (two.x * one.y); 
}

int compDist(Point one, Point two) {
	int dot = prodDot(one, two);
	if(dot == 0) return 0;
	if(dot > 0) return -1;
	return 1;
}

int compAngle(Point one, Point two) {
	int cross = prodCross(one, two);
	if(cross == 0) return 0;
	if(cross > 0) return -1;
	return 1;
}

int rad(const void* pOne, const void* pTwo) {
	Room rOne = *(const Room*) pOne;
	Room rTwo = *(const Room*) pTwo;
	
	Point one = {
		rOne.pos.x + rOne.dim.x/2,
		rOne.pos.y + rOne.dim.y/2,
	};
	
	Point two = {
		rTwo.pos.x + rTwo.dim.x/2,
		rTwo.pos.y + rTwo.dim.y/2,
	};
	
	if(one.x == two.x && one.y == two.y) return 0;
	if(one.x == 0 && one.y == 0) return -1;
	if(two.x == 0 && two.y == 0) return 1;
	if(compAngle(one, two) > 0) return 1;
	if(compAngle(one, two) < 0) return -1;
	if(compDist(one, two) < 0) return -1;
	return 1;
}

void hallPlace(Dungeon dungeon, Point point) {
	if (point.x >= 0 
		&& point.y >= 0
		&& point.x < dungeon.dim.x
		&& point.y < dungeon.dim.y) {
		Tile* tile = &dungeon.tiles[point.y][point.x];
		if (tile->type == ROCK) {
			tile->type = HALL;
			tile->symbol = SYM_HALL;
		}
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

void roomConnectRasterize(Dungeon dungeon, Point from, Point to) {
	Point current = from;
	
	Point dist = {
		abs(to.x - from.x),
		-abs(to.y - from.y)
	};
	
	Point step = {
		(from.x < to.x ? 1 : -1),
		(from.y < to.y ? 1 : -1)
	};
	
	int error = dist.x + dist.y;
	
	hallPlace(dungeon, current);
	
	while (current.x != to.x || current.y != to.y) {
		if (2 * error - dist.y > dist.x - 2 * error) {
			// horizontal step
			error += dist.y;
			current.x += step.x;
		} else {
			// vertical step
			error += dist.x;
			current.y += step.y;
		}
		
		hallPlace(dungeon, current);
	}
}

void roomConnect(Dungeon dungeon, Room first, Room second) {
	Point start = {
		rand() % first.dim.x + first.pos.x,
		rand() % first.dim.y + first.pos.y,
	};
	
	Point end = {
		rand() % second.dim.x + second.pos.x,
		rand() % second.dim.y + second.pos.y,
	};
	
	Point offset = {
		(rand() % (ROOM_CON_RAD + 1 - ROOM_CON_RAD_MIN) + ROOM_CON_RAD_MIN),
		(rand() % (ROOM_CON_RAD + 1 - ROOM_CON_RAD_MIN) + ROOM_CON_RAD_MIN),
	};
	
	offset.x *= rand() % 2 ? 1 : -1;
	offset.y *= rand() % 2 ? 1 : -1;
	
	Point mid = {
		(((float)(start.x) + (float)(end.x)) / 2.0f + (float)offset.x),
		(((float)(start.y) + (float)(end.y)) / 2.0f + (float)offset.y)
	};
	
	if(mid.x < 1) mid.x = 1;
	if(mid.y < 1) mid.y = 1;
	if(mid.x > dungeon.dim.x - 2) mid.x = dungeon.dim.x - 2;
	if(mid.y > dungeon.dim.y - 2) mid.y = dungeon.dim.y - 2;
	
	roomConnectRasterize(dungeon, start, mid);
	roomConnectRasterize(dungeon, mid, end);
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

void dungeonDrawNorthSouth(Dungeon dungeon, const wchar_t* sym, int row) {
	for(int col = 0; col < dungeon.dim.x; col++) {
		dungeon.tiles[row][col] = (Tile){
			sym,
			EDGE
		};
	}
}

void dungeonDrawEastWest(Dungeon dungeon, const wchar_t* sym, int col) {
	for(int row = 0; row < dungeon.dim.y; row++) {
		dungeon.tiles[row][col] = (Tile){
			sym,
			EDGE
		};
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
	
	//Initialize dungeon
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
	dungeonDrawNorthSouth(dungeon, SYM_EDGE_N, 0);
	dungeonDrawEastWest(dungeon, SYM_EDGE_E, dim.x - 1);
	dungeonDrawNorthSouth(dungeon, SYM_EDGE_S, dim.y - 1);
	dungeonDrawEastWest(dungeon, SYM_EDGE_W, 0);

	//Draw corners
	dungeon.tiles[0][0].symbol = SYM_EDGE_NW;
	dungeon.tiles[0][dim.x - 1].symbol = SYM_EDGE_NE;
	dungeon.tiles[dim.y - 1][dim.x - 1].symbol = SYM_EDGE_SE;
	dungeon.tiles[dim.y - 1][0].symbol = SYM_EDGE_SW;
	
	Room rooms[ROOMS_MAX];
	int count = 0;
	
	//Create rooms
	for(count = 0; count < ROOMS_MAX; count++) {
		if (count >= ROOMS_MIN && dungeonIsFull(dungeon)) break;
		rooms[count] = roomGenerate(dungeon);
		roomPlace(dungeon, rooms[count]);
	}
	
	//Normalize
	for(int i = 0; i < count; i++) {
		rooms[i].pos.x -= (dungeon.dim.x)/2;
		rooms[i].pos.y -= (dungeon.dim.y)/2;
	}
	
	//Sort (for consistency)
	qsort(rooms, count, sizeof(Room), rad);
	
	//Revert
	for(int i = 0; i < count; i++) {
		rooms[i].pos.x += (dungeon.dim.x)/2;
		rooms[i].pos.y += (dungeon.dim.y)/2;
	}
	
	//Create the paths.
	for(int first = 0; first < count; first++) {
		roomConnect(dungeon, rooms[first], rooms[first + 1 == count ? 0 : first + 1]);
	}
	
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
