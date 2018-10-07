#define _XOPEN_SOURCE_EXTENDED

#include <wchar.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <endian.h>
#include <ncursesw/curses.h>

#include "mob.h"
#include "dungeon.h"
#include "perlin.h"
#include "point.h"
#include "path.h"
#include "main.h"

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
const wchar_t SYM_EDGE_N = L'\x2584';
const wchar_t SYM_EDGE_E = L'\x2588';
const wchar_t SYM_EDGE_S = L'\x2580';
const wchar_t SYM_EDGE_W = L'\x2588';

//Rocks
const wchar_t SYM_ROCK_SOFT = L'\x2591';
const wchar_t SYM_ROCK_MED  = L'\x2592';
const wchar_t SYM_ROCK_HARD = L'\x2593';

//Hall
const wchar_t SYM_HALL = L'\x2022';
const wchar_t SYM_HALL_NESW = L' ';
const wchar_t SYM_HALL_NES = L'\x2560';
const wchar_t SYM_HALL_ESW = L'\x2566';
const wchar_t SYM_HALL_SWN = L'\x2563';
const wchar_t SYM_HALL_WNE = L'\x2569';
const wchar_t SYM_HALL_NE = L'\x255A';
const wchar_t SYM_HALL_ES = L'\x2554';
const wchar_t SYM_HALL_SW = L'\x2557';
const wchar_t SYM_HALL_WN = L'\x255D';
const wchar_t SYM_HALL_NS = L'\x2551';
const wchar_t SYM_HALL_EW = L'\x2550';

//Rooms are air
const wchar_t SYM_ROOM = L' ';
const wchar_t SYM_VOID = L'x';

//Magic header
const char* HEADER = "RLG327-F2018";
const uint32_t VERSION = 0;

//Default Size
const Point DUNGEON_DIM = {80,21};

static void hallPlace(Dungeon dungeon, Point point) {
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

static int roomPlaceAttempt(Dungeon dungeon, Room room) {
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

static void roomPlace(Dungeon dungeon, Room room) {
	for (int row = room.pos.y; row < room.pos.y + room.dim.y; row++) {
		for (int col = room.pos.x; col < room.pos.x + room.dim.x; col++) {
			Tile* tile = &dungeon.tiles[row][col];
			tile->type = ROOM;
			tile->symbol = SYM_ROOM;
			tile->hardness = 0;
		}
	}
}

static void roomConnectRasterize(Dungeon dungeon, Point from, Point to) {
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

static void roomConnect(Dungeon dungeon, Room first, Room second) {
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
	mid.x = (int)(((float)(start.x) + (float)(end.x)) / 2.0f + (float)offset.x);
	mid.y = (int)(((float)(start.y) + (float)(end.y)) / 2.0f + (float)offset.y);
	
	//Check bounds of midpoint.
	if(mid.x < 1) mid.x = 1;
	if(mid.y < 1) mid.y = 1;
	if(mid.x > dungeon.dim.x - 2) mid.x = dungeon.dim.x - 2;
	if(mid.y > dungeon.dim.y - 2) mid.y = dungeon.dim.y - 2;

	roomConnectRasterize(dungeon, start, mid);
	roomConnectRasterize(dungeon, mid, end);
}

static Room roomGenerate(Dungeon dungeon) {
	Point dim = {0};
	Point pos = {0};
	Room room;
	
	do {
		dim.x = utilSkewedBetweenRange(ROOM_X_MIN, ROOM_X_MAX);
		dim.y = utilSkewedBetweenRange(ROOM_Y_MIN, ROOM_Y_MAX);
		pos.x = (rand() % (dungeon.dim.x - 1 - dim.x)) + 1;
		pos.y = (rand() % (dungeon.dim.y - 1 - dim.y)) + 1;
		room = (Room){pos, dim};
	} while (!roomPlaceAttempt(dungeon, room));

	return room;
}

static Tile tileGenerate(Point dim, Point pos, uint8_t hardness, const float* seed) {
	Tile tile = {0};
	
	//Defaults
	tile.symbol = SYM_VOID;
	tile.type = VOID;
	tile.hardness = 0xFF;

	if (seed != NULL) {
		//We were given a seed to generate the map
		if (pos.y == 0 || pos.y == dim.y - 1 || pos.x == 0 || pos.x == dim.x - 1) {
			tile.type = EDGE;
			tile.hardness = 0xFF;
		} else {
			tile.type = ROCK;
			tile.hardness = (uint8_t)(noise2D(dim, pos, seed, 4, 0.2f) * 255.0f);
		}
	} else if (hardness == 0xFF) {
		//Edge if hardness is 255.
		tile.type = EDGE;
		tile.hardness = 0xFF;
	} else if (hardness == 0x00) {
		//Hallway if hardness is 0. Will convert rooms to rooms during load.
		tile.type = HALL;
		tile.hardness = 0x00;
	} else {
		//If not max hardness or not min hardness, then it's a rock.
		tile.type = ROCK;
		tile.hardness = hardness;
	}
	
	return tile;
}

static int dungeonIsFull(Dungeon dungeon) {
	float total = dungeon.dim.x * dungeon.dim.y;
	float room = 0.0;
	for(int row = 0; row < dungeon.dim.y; row++) {
		for(int col = 0; col < dungeon.dim.x; col++) {
			if (dungeon.tiles[row][col].type == ROOM) room = room + 1.0f;
		}
	}
	
	return room/total > ROOM_MAX_FULLNESS;
}

static void dungeonFinalize(Dungeon *dungeon, int mobs, int emoji) {
	//Setup buffer
	size_t len = (size_t) (dungeon->dim.x) + 1;
	dungeon->status = calloc(len, sizeof(wchar_t));
	dungeon->line1 = calloc(len, sizeof(wchar_t));
	dungeon->line2 = calloc(len, sizeof(wchar_t));

	//Create mobs
	dungeon->numMobs = mobs;
	dungeon->mobs = mobGenerateAll(*dungeon);
	dungeon->emoji = emoji;
}

static void dungeonPostProcess(Dungeon dungeon) {
	for(int row = 0; row < dungeon.dim.y; row++) {
		for(int col = 0; col < dungeon.dim.x; col++) {
			Tile* tile = &dungeon.tiles[row][col];

			//Render rock hardness
			if (tile->type == ROCK) {
				if (tile->hardness < 0x55) {
					tile->symbol = SYM_ROCK_SOFT;
				} else if (tile->hardness < 0xAA) {
					tile->symbol = SYM_ROCK_MED;
				} else {
					tile->symbol = SYM_ROCK_HARD;
				}
			}

			//Render the edge
			if (tile->type == EDGE) {
				if (row == 0) {
					tile->symbol = SYM_EDGE_N;
				} else if (row == dungeon.dim.y - 1) {
					tile->symbol = SYM_EDGE_S;
				} else if (col == 0) {
					tile->symbol = SYM_EDGE_W;
				} else if (col == dungeon.dim.x - 1) {
					tile->symbol = SYM_EDGE_E;
				}
			}

			if (tile->type == ROOM) {
				tile->symbol = SYM_ROOM;
			}

			if (tile->type != HALL) continue;

			Tile* tileN = &dungeon.tiles[row - 1][col];
			Tile* tileE = &dungeon.tiles[row][col + 1];
			Tile* tileS = &dungeon.tiles[row + 1][col];
			Tile* tileW = &dungeon.tiles[row][col - 1];
			
			int NESW = 0x000F
				& (((tileN->type == HALL || tileN->type == ROOM) << 3) 
				| ((tileE->type == HALL || tileE->type == ROOM) << 2)
				| ((tileS->type == HALL || tileS->type == ROOM) << 1)
				| (tileW->type == HALL || tileW->type == ROOM));
			
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
				case 0x08: //1000
				case 0x02: //0010
					tile->symbol = SYM_HALL_NS;
					break;
				case 0x04: //0100
				case 0x01: //0001
					tile->symbol = SYM_HALL_EW;
					break;
				default:  //No chars for stubs.
					tile->symbol = SYM_HALL;
			}
		}
	}
}

//"Public" functions

void setText(Dungeon dungeon, wchar_t** buffer, wchar_t* text) {
	//Reset buffer
	size_t len = (size_t)(dungeon.dim.x) + 1;
	swprintf(*buffer, len, L"%-*ls", len, text);
}

Dungeon dungeonGenerate(Point dim, int mobs, int emoji) {
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
	
	//Normalize
	for(int i = 0; i < count; i++) {
		rooms[i].pos.x -= (dungeon.dim.x)/2;
		rooms[i].pos.y -= (dungeon.dim.y)/2;
	}
	
	//Sort (for consistency)
	qsort(rooms, (size_t)count, sizeof(Room), utilSortRad);
	
	//Revert
	for(int i = 0; i < count; i++) {
		rooms[i].pos.x += (dungeon.dim.x)/2;
		rooms[i].pos.y += (dungeon.dim.y)/2;
	}

	//Place player
	dungeon.player = mobGeneratePlayer((Point){
		rooms[0].pos.x + (rooms[0].dim.x)/2,
		rooms[0].pos.y + (rooms[0].dim.y)/2
	});

	//Create the paths.
	roomConnect(dungeon, rooms[0], rooms[count - 1]);
	for(int first = 0; first < count - 1; first++) {
		roomConnect(dungeon, rooms[first], rooms[first + 1]);
	}
	
	//Copy rooms into structure
	dungeon.numRooms = count;
	dungeon.rooms = malloc(sizeof(Room) * count);
	memcpy(dungeon.rooms, rooms, sizeof(Room) * count);
	
	//Finalize dungeon
	dungeonFinalize(&dungeon, mobs, emoji);
	
	return dungeon;
}

Dungeon dungeonLoad(FILE* file, int mobs, int emoji) {
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
	version = be32toh(version);
	if (read != 1 || version != VERSION) {
		wprintf(L"Bad file version!\n");
		exit(FILE_READ_BAD_VERSION);
	}
	
	//Get file size
	uint32_t size;
	read = fread(&size, sizeof(uint32_t), 1, file);
	size = be32toh(size);
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

	dungeon.player = mobGeneratePlayer((Point){xPlayer, yPlayer});
	
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
	int rooms = (int) ((size - ftell(file)) / 4l);
	rooms = rooms > 0 ? rooms : 0;
	dungeon.rooms = malloc(sizeof(Room) * rooms);
	dungeon.numRooms = rooms;
	
	//Get rooms
	while ((uint32_t)(ftell(file)) < size) {
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

	//Finalize dungeon
	dungeonFinalize(&dungeon, mobs, emoji);
	
	return dungeon;
}

void dungeonSave(Dungeon dungeon, FILE* file) {
	//Header
	fwrite(HEADER, sizeof(char), strlen(HEADER), file);
	
	//Version
	uint32_t ver = htobe32(VERSION);
	fwrite(&ver, sizeof(uint32_t), 1, file);
	
	//Size of file, 22 for the header, width and height of dungeon, and extra rooms
	uint32_t size = htobe32((uint32_t)(22 + dungeon.dim.x * dungeon.dim.y + dungeon.numRooms * 4));
	fwrite(&size, sizeof(uint32_t), 1, file);
	
	//Player position
	uint8_t playerX = (uint8_t) dungeon.player.pos.x;
	uint8_t playerY = (uint8_t) dungeon.player.pos.y;
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

void dungeonDestroy(Dungeon* dungeon) {
	pathDestroy(dungeon);

	for(int row = 0; row < dungeon->dim.y; row++) {
		free(dungeon->tiles[row]);
	}

	free(dungeon->status);
	free(dungeon->line1);
	free(dungeon->line2);
	free(dungeon->mobs);
	free(dungeon->tiles);
	free(dungeon->rooms);
	dungeon->tiles = NULL;
	dungeon->rooms = NULL;
	dungeon->dim = (Point){0};
	dungeon->numRooms = 0;
	dungeon->numMobs = 0;
}

void dungeonPrint(WINDOW* win, Dungeon dungeon) {
	//Make the dungeon look nice
	dungeonPostProcess(dungeon);

	//Write status
	mvwaddwstr(win, 0, 0, dungeon.status);

	//Write tiles
	for (int row = 0; row < dungeon.dim.y; row++) {
		wchar_t screen[dungeon.dim.x + 1];
		for (int col = 0; col < dungeon.dim.x; col++) {
			screen[col] = dungeon.tiles[row][col].symbol;
		}

		screen[dungeon.dim.x] = L'\0';
		mvwaddwstr(win, row + 1, 0, screen);
	}

	//Write mobs
	mvwaddwstr(win, dungeon.player.pos.y + 1, dungeon.player.pos.x, SYM_PLAY);
	for (int mob = 0; mob < dungeon.numMobs; mob++) {
		Mob m = dungeon.mobs[mob];
		if (m.hp > 0) mvwaddwstr(win, m.pos.y + 1, m.pos.x, mobGetSymbol(&m, dungeon));
	}

	//Write other statuses
	mvwaddwstr(win, dungeon.dim.y + 1, 0, dungeon.line1);
	mvwaddwstr(win, dungeon.dim.y + 2, 0, dungeon.line2);

	//Flip screen
	wrefresh(win);
}
