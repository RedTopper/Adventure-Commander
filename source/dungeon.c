#include <wchar.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <byteswap.h>

#include "types.h"
#include "dungeon.h"
#include "perlin.h"
#include "util.h"

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

//Edges across board
const wchar_t* SYM_EDGE = L"\x2588";
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
const wchar_t* SYM_VOID = L"x";

//Magic header
const char* HEADER = "RLG327-F2018";
const uint32_t VERSION = 0;

void hallPlace(Dungeon dungeon, Point point) {
	if (point.x >= 0 
		&& point.y >= 0
		&& point.x < dungeon.dim.x
		&& point.y < dungeon.dim.y) {
		Tile* tile = &dungeon.tiles[point.y][point.x];
		if (tile->type == ROCK) {
			tile->type = HALL;
			tile->symbol = SYM_HALL;
			tile->hardness = 0;
		}
	}
}

int roomPlaceAttempt(Dungeon dungeon, Room room) {
	for (int row = room.pos.y - 1; row < room.pos.y + room.dim.y + 1; row++) {
		for (int col = room.pos.x - 1; col < room.pos.x + room.dim.x + 1; col++) {
			if (row < 0) return 0;
			if (col < 0) return 0;
			if (row >= dungeon.dim.y) return 0;
			if (col >= dungeon.dim.x) return 0;
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
			tile->hardness = 0;
		}
	}
}

void roomConnectRasterize(Dungeon dungeon, Point from, Point to) {
	Point current = from;
	Point dist = {0};
	Point step = {0};
	
	dist.x = abs(to.x - from.x);
	dist.y = -abs(to.y - from.y);
	step.x = (from.x < to.x ? 1 : -1);
	step.y = (from.y < to.y ? 1 : -1);
	
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
	Point start = {0};
	Point end = {0};
	Point offset = {0};
	Point mid = {0};
	
	//Start anywhere in the first room
	start.x = rand() % first.dim.x + first.pos.x;
	start.y = rand() % first.dim.y + first.pos.y;
	
	//End anywhere in the second room
	end.x = rand() % second.dim.x + second.pos.x;
	end.y = rand() % second.dim.y + second.pos.y;
	
	//Find some value to offset the midpoint.
	offset.x = (rand() % (ROOM_CON_RAD + 1 - ROOM_CON_RAD_MIN) + ROOM_CON_RAD_MIN);
	offset.y = (rand() % (ROOM_CON_RAD + 1 - ROOM_CON_RAD_MIN) + ROOM_CON_RAD_MIN);
	offset.x *= rand() % 2 ? 1 : -1;
	offset.y *= rand() % 2 ? 1 : -1;
	
	//Get the actual midpoint.
	mid.x = (((float)(start.x) + (float)(end.x)) / 2.0f + (float)offset.x);
	mid.y = (((float)(start.y) + (float)(end.y)) / 2.0f + (float)offset.y);
	
	//Check bounds of midpoint.
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
		dim.x = utilSkewedBetweenRange(ROOM_X_MIN, ROOM_X_MAX);
		dim.y = utilSkewedBetweenRange(ROOM_Y_MIN, ROOM_Y_MAX);
		pos.x = (rand() % (dungeon.dim.x - 1 - dim.x)) + 1;
		pos.y = (rand() % (dungeon.dim.y - 1 - dim.y)) + 1;
		room = (Room){pos, dim};
	} while (!roomPlaceAttempt(dungeon, room));

	return room;
}

Tile tileGenerate(Point dim, Point pos, uint8_t hardness, float* seed) {
	Tile tile = {0};
	
	//Defaults
	tile.symbol = SYM_VOID;
	tile.type = VOID;
	tile.hardness = 0x00;
	
	if (hardness == 0xFF || seed != NULL) {
		//Edge if hardness is 255. There is a case when we want
		//to generate rocks, which is when the seed is not null.
		tile.type = EDGE;
		tile.hardness = 0xFF;
		tile.symbol = SYM_EDGE;
		
		if (pos.y == 0) {
			tile.symbol = SYM_EDGE_N;
		} else if (pos.y == dim.y - 1) {
			tile.symbol = SYM_EDGE_S;
		} else if (pos.x == 0) {
			tile.symbol = SYM_EDGE_W;
		} else if (pos.x == dim.x - 1) {
			tile.symbol = SYM_EDGE_E;
		} else if (seed != NULL) {
			//In this case, we are generating the rooms from scratch, since
			//the seed value isn't passed when loading from a file.
			//Populate the rest of the tiles with rock.
			tile.type = ROCK;
			tile.hardness = (noise2D(dim, pos, seed, 4, 0.2f) * 255.0f);
		}
	} else if (hardness == 0x00) {
		//Hallway if hardness is 0. Will convert rooms to rooms
		//when the last bit of file is loaded.
		tile.symbol = SYM_HALL;
		tile.type = HALL;
		tile.hardness = 0x00;
	} else {
		//If not max hardness or not min hardness, then it's a rock.
		tile.type = ROCK;
		tile.hardness = hardness;
	}
	
	if (tile.type == ROCK) {
		//If a tile is a rock, change it's look depending on hardness.
		if (tile.hardness < 0x55) {
			tile.symbol = SYM_ROCK_SOFT;
		} else if (tile.hardness < 0xAA) {
			tile.symbol = SYM_ROCK_MED;
		} else {
			tile.symbol = SYM_ROCK_HARD;
		}
	}
	
	return tile;
}

int dungeonIsFull(Dungeon dungeon) {
	float total = dungeon.dim.x * dungeon.dim.y;
	float room = 0.0;
	for(int row = 0; row < dungeon.dim.y; row++) {
		for(int col = 0; col < dungeon.dim.x; col++) {
			if (dungeon.tiles[row][col].type == ROOM) room = room + 1.0f;
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
			
			int NESW = 0x000F
				& (((tileN->type == HALL || tileN->type == ROOM) << 3) 
				| ((tileE->type == HALL || tileE->type == ROOM) << 2)
				| ((tileS->type == HALL || tileS->type == ROOM) << 1)
				| (tileW->type == HALL || tileW->type == ROOM));
				
			if (tile->type != HALL) continue;
			
			switch (NESW) {
				case 0x0F: //1111
					tile->symbol = SYM_HALL_NESW;
					break;
				case 0x0E: //1110
					tile->symbol = SYM_HALL_NES;
					break;
				case 0x07: //0111
					tile->symbol = SYM_HALL_ESW;
					break;
				case 0x0B: //1011
					tile->symbol = SYM_HALL_SWN;
					break;
				case 0x0D: //1101
					tile->symbol = SYM_HALL_WNE;
					break;
				case 0x0C: //1100
					tile->symbol = SYM_HALL_NE;
					break;
				case 0x06: //0110
					tile->symbol = SYM_HALL_ES;
					break;
				case 0x03: //0011
					tile->symbol = SYM_HALL_SW;
					break;
				case 0x09: //1001
					tile->symbol = SYM_HALL_WN;
					break;
				case 0x0A: //1010
					tile->symbol = SYM_HALL_NS;
					break;
				case 0x05: //0101
					tile->symbol = SYM_HALL_EW;
					break;
				default:  //No chars for stubs.
					tile->symbol = SYM_HALL;
			}
		}
	}
}

//"Public" functions

Dungeon dungeonGenerate(Point dim) {
	Dungeon dungeon = {0};
	dungeon.dim = dim;
	
	if (dim.x < 1 || dim.y < 1) return dungeon;
	
	//Get seed for perlin noise
	float seed[dim.y * dim.x];
	for (int i = 0; i < dim.y * dim.x; i++) {
		seed[i] = (float)rand() / (float)RAND_MAX;
	}
	
	//Initialize dungeon
	dungeon.tiles = (Tile**) malloc(sizeof(Tile*) * dim.y);
	for(int row = 0; row < dim.y; row++) {
		dungeon.tiles[row] = malloc(sizeof(Tile) * dim.x);
		for(int col = 0; col < dim.x; col++) {
			dungeon.tiles[row][col] = tileGenerate(dim, (Point){col, row}, 0x01, seed);
		}
	}
	
	Room rooms[ROOMS_MAX];
	int count = 0;
	
	//Create rooms
	for(count = 0; count < ROOMS_MAX; count++) {
		if (count >= ROOMS_MIN && dungeonIsFull(dungeon)) break;
		rooms[count] = roomGenerate(dungeon);
		roomPlace(dungeon, rooms[count]);
	}
	
	//Place player 
	dungeon.player.x = rooms[0].pos.x + (rooms[0].dim.x)/2;
	dungeon.player.y = rooms[0].pos.y + (rooms[0].dim.y)/2;
	
	//Normalize
	for(int i = 0; i < count; i++) {
		rooms[i].pos.x -= (dungeon.dim.x)/2;
		rooms[i].pos.y -= (dungeon.dim.y)/2;
	}
	
	//Sort (for consistency)
	qsort(rooms, count, sizeof(Room), utilSortRad);
	
	//Revert
	for(int i = 0; i < count; i++) {
		rooms[i].pos.x += (dungeon.dim.x)/2;
		rooms[i].pos.y += (dungeon.dim.y)/2;
	}
	
	//Create the paths.
	for(int first = 0; first < count - 1; first++) {
		roomConnect(dungeon, rooms[first], rooms[first + 1]);
	}
	
	//Additional connections so the player can move easier. 
	roomConnect(dungeon, rooms[0], rooms[count - 1]);
	//roomConnect(dungeon, rooms[0], rooms[(count - 1)/2]);
	
	//Copy rooms into structure
	dungeon.numRooms = count;
	dungeon.rooms = malloc(sizeof(Room) * count);
	memcpy(dungeon.rooms, rooms, sizeof(Room) * count);
	
	//Prettify the dungeon.
	dungeonPostProcess(dungeon);
	
	return dungeon;
}

Dungeon dungeonLoad(FILE* file) {
	size_t length = strlen(HEADER);
	char head[length + 1];
	size_t read = 0;
	Dungeon dungeon = {0};
	dungeon.dim = DUNGEON_DIM;
	
	//Read magic header
	head[length] = '\0';
	read = fread(head, sizeof(char), length, file);
	if (read != length || strcmp(head, HEADER) != 0) {
		wprintf(L"Bad file header '%s'!\n", head);
		exit(FILE_READ_BAD_HEAD);
	}
	
	//Check file version
	uint32_t version;
	read = fread(&version, sizeof(uint32_t), 1, file);
	version = __bswap_32 (version); //Big to little
	if (read != 1 || version != VERSION) {
		wprintf(L"Bad file version!\n");
		exit(FILE_READ_BAD_VERSION);
	}
	
	//Get file size
	uint32_t size;
	read = fread(&size, sizeof(uint32_t), 1, file);
	size = __bswap_32 (size); //Big to little
	if (read != 1) {
		wprintf(L"Bad file size (EOF)!\n");
		exit(FILE_READ_EOF_SIZE);
	}
	
	//Get position
	uint8_t xPlayer;
	uint8_t yPlayer;
	read = fread(&xPlayer, sizeof(uint8_t), 1, file);
	read += fread(&yPlayer, sizeof(uint8_t), 1, file);
	if (read != 2 || xPlayer >= dungeon.dim.x || yPlayer >= dungeon.dim.y) {
		wprintf(L"Bad file player co-ordinates (EOF)!\n");
		exit(FILE_READ_EOF_PLAYER);
	}
	
	dungeon.player.x = xPlayer;
	dungeon.player.y = yPlayer;
	
	//Initialize dungeon
	dungeon.tiles = (Tile**) malloc(sizeof(Tile*) * dungeon.dim.y);
	for(int row = 0; row < dungeon.dim.y; row++) {
		dungeon.tiles[row] = malloc(sizeof(Tile) * dungeon.dim.x);
		for(int col = 0; col < dungeon.dim.x; col++) {
			uint8_t hardness;
			read = fread(&hardness, sizeof(uint8_t), 1, file);
			
			if (read != 1) {
				wprintf(L"Missing tile harness information (EOF)!\n");
				exit(FILE_READ_EOF_HARDNESS);
			}
			
			dungeon.tiles[row][col] = tileGenerate(dungeon.dim, (Point){col, row}, hardness, NULL);
		}
	}
	
	int i = 0;
	int rooms = (size - ftell(file))/4l;
	rooms = rooms > 0 ? rooms : 0;
	dungeon.rooms = malloc(sizeof(Room) * rooms);
	dungeon.numRooms = rooms;
	
	//Get rooms
	while (ftell(file) < size) {
		Room room;
		uint8_t posX;
		uint8_t posY;
		uint8_t dimX;
		uint8_t dimY;
		read = fread(&posX, sizeof(uint8_t), 1, file);
		read += fread(&posY, sizeof(uint8_t), 1, file);
		read += fread(&dimX, sizeof(uint8_t), 1, file);
		read += fread(&dimY, sizeof(uint8_t), 1, file);
		
		if (read != 4) {
			wprintf(L"Missing rooms information (EOF)!\n");
			exit(FILE_READ_EOF_ROOMS);
		}
		
		//Verify the rooms are in bounds. If they are not, throw them out.
		room.pos.x = posX >= dungeon.dim.x ? dungeon.dim.x - 1 : posX;
		room.pos.y = posY >= dungeon.dim.x ? dungeon.dim.x - 1 : posY;
		room.dim.x = dimX + room.pos.x >= dungeon.dim.x ? 1 : dimX;
		room.dim.y = dimY + room.pos.y >= dungeon.dim.x ? 1 : dimY;
		
		roomPlace(dungeon, room);
		dungeon.rooms[i++] = room;
	}
	
	//Prettify the dungeon.
	dungeonPostProcess(dungeon);
	
	return dungeon;
}

void dungeonSave(Dungeon dungeon, FILE* file) {
	//Header
	fwrite(HEADER, sizeof(char), strlen(HEADER), file);
	
	//Version
	uint32_t ver = __bswap_32(VERSION);
	fwrite(&ver, sizeof(uint32_t), 1, file);
	
	//Size of file, 22 for the header, width and height of dungeon, and extra rooms
	uint32_t size = __bswap_32(22 + dungeon.dim.x * dungeon.dim.y + dungeon.numRooms * 4);
	fwrite(&size, sizeof(uint32_t), 1, file);
	
	//Player position
	uint8_t playerX = dungeon.player.x;
	uint8_t playerY = dungeon.player.y;
	fwrite(&playerX, sizeof(uint8_t), 1, file);
	fwrite(&playerY, sizeof(uint8_t), 1, file);
	
	//Tiles
	for(int row = 0; row < dungeon.dim.y; row++) {
		for(int col = 0; col < dungeon.dim.x; col++) {
			uint8_t hardness = dungeon.tiles[row][col].hardness;
			fwrite(&hardness, sizeof(uint8_t), 1, file);
		}
	}
	
	//Rooms
	for(int i = 0; i < dungeon.numRooms; i++) {
		Room room = dungeon.rooms[i];
		uint8_t posX = (uint8_t)(room.pos.x);
		uint8_t posY = (uint8_t)(room.pos.y);
		uint8_t dimX = (uint8_t)(room.dim.x);
		uint8_t dimY = (uint8_t)(room.dim.y);
		fwrite(&posX, sizeof(uint8_t), 1, file);
		fwrite(&posY, sizeof(uint8_t), 1, file);
		fwrite(&dimX, sizeof(uint8_t), 1, file);
		fwrite(&dimY, sizeof(uint8_t), 1, file);
	}
}

void dungeonDestroy(Dungeon dungeon) {
	for(int row = 0; row < dungeon.dim.y; row++) {
		free(dungeon.tiles[row]);
	}
	
	free(dungeon.tiles);
	free(dungeon.rooms);
	dungeon.dim = (Point){0};
	dungeon.numRooms = 0;
}

void dungeonPrint(Dungeon dungeon) {
	for(int row = 0; row < dungeon.dim.y; row++) {
		for(int col = 0; col < dungeon.dim.x; col++) {
			wprintf(L"%ls", dungeon.tiles[row][col].symbol);
		}
		wprintf(L"\n");
	}
}

void dungeonPrintDebug(Dungeon dungeon) {
	for(int row = 0; row < dungeon.dim.y; row++) {
		for(int col = 0; col < dungeon.dim.x; col++) {
			switch (dungeon.tiles[row][col].type) {
				case VOID:
					wprintf(L"x");
					break;
				case EDGE:
					wprintf(L"e");
					break;
				case ROCK:
					wprintf(L"o");
					break;
				case ROOM:
					wprintf(L"r");
					break;
				case HALL:
					wprintf(L"h");
					break;
			}
		}
		wprintf(L"\n");
	}
}
