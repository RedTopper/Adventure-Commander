#include <wchar.h>
#include <locale.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include "mob.h"
#include "main.h"
#include "dungeon.h"
#include "path.h"

const char* SAVE = "--save";
const char* LOAD = "--load";
const char* MOBS = "--nummon";

static volatile int running = 1;
static void sig() {
	wprintf(L"Caught CTRL-C: Quitting!\n");
	fflush(stdout);
	running = 0;
}

static void help(char* message, char* command, Error error ) {
	wprintf(L"%s: ", command);
	wprintf(L"%s\n", message);
	wprintf(
		L"Adventure Commander Help: \n"
		L"[--load] Load a file from ~/.rlg327/dungeon\n"
		L"[--save] Write a dungeon to ~/.rlg327/dungeon\n"
		L"[--nummon] <count> Number of monsters to generate\n"
	);
	exit(error);
}

static int require(int* on, int count, char* command) {
	if ((*on) < count - 1) {
		(*on)++;
	} else {
		help("This param requires another argument", command, ARGUMENT_REQ_PARAM);
	}

	return 1;
}

static FILE* get(char* mode) {
	char path[256];
	char* home = getenv("HOME");
	snprintf(path, 255, "%s/.rlg327/dungeon", home);

	//Load dungeon from file
	FILE* file = fopen(path, mode);
	if (file == NULL) {
		help("Failed to open file", path, FILE_READ_GONE);
	}

	return file;
}

int main(int argc, char** argv) {
	setlocale(LC_ALL, "");
	srand((unsigned int) time(NULL));
	signal(SIGINT, sig);
	
	int save = 0;
	int load = 0;
	int mobs = 10;
	
	//Parse arguments
	for (int arg = 1; arg < argc; arg++) {
		if (strlen(argv[arg]) < 3 || argv[arg][0] != '-' || argv[arg][1] != '-') {
			//Check length
			help("Bad argument", argv[arg], ARGUMENT_NO_DASH);
		} else {
			//Check if correct argument
			if (strcmp(SAVE, argv[arg]) == 0) {
				save = 1;
			} else if (strcmp(LOAD, argv[arg]) == 0) {
				load = 1;
			} else if (strcmp(MOBS, argv[arg]) == 0 && require(&arg, argc, argv[arg])) {
				mobs = atoi(argv[arg]); // NOLINT(cert-err34-c)
				if (mobs < 1) help("Must be between 1-100", (char*)MOBS, ARGUMENT_OOB);
				if (mobs > 100) help("Must be between 1-100", (char*)MOBS, ARGUMENT_OOB);
			} else {
				help("Unknown argument", argv[arg], ARGUMENT_UNKNOWN);
			}
		}
	}

	Dungeon dungeon;

	//Load dungeon from file
	if (load) {
		FILE* file = get("rb");
		dungeon = dungeonLoad(file, mobs);
		fclose(file);
	} else {
		dungeon = dungeonGenerate(DUNGEON_DIM, mobs);
	}

	//Main loop
	setBufferPad(&dungeon.line1, L"Adventure Commander", (size_t)dungeon.dim.x + 1);
	setBufferPad(&dungeon.line2, L"Nothing has happened yet!", (size_t)dungeon.dim.x + 1);
	setBuffer(&dungeon.prompt, L"(Disabled) > ", (size_t)dungeon.dim.x + 1);
	wprintf(L"\033[H\033[J");
	pathCreate(&dungeon);
	Node* turn = mobCreateQueue(&dungeon);
	while (running && dungeon.player.hp > 0 && mobAliveCount(dungeon)) {
		wprintf(L"\033[0;0H\n\033[0;0H");
		int priority = queuePeekPriority(&turn);
		Mob* mob = queuePeek(&turn).mob;
		queuePop(&turn);
		mobTick(&dungeon, mob);
		queuePushSub(&turn, (NodeData){.mob=mob}, priority + 1000/mob->speed, mob->order);
		dungeonPostProcess(dungeon);
		dungeonPrint(dungeon);
		usleep(200000);
	}

	if (running) {
		if (dungeon.player.hp == 0) {
			wprintf(L"You died. Better luck next time!\n");
		} else {
			wprintf(L"CONGLATURATION !!!\n");
		}
	}

	//Save dungeon to file.
	if (save) {
		FILE* file = get("wb");
		dungeonSave(dungeon, file);
		fclose(file);
	}

	queueDestroy(&turn);
	dungeonDestroy(&dungeon);
}

//"Public" functions

void setBufferPad(wchar_t** buffer, wchar_t* text, size_t length) {
	swprintf(*buffer, length, L"%-*ls", length, text);
}

void setBuffer(wchar_t** buffer, wchar_t* text, size_t length) {
	swprintf(*buffer, length, L"%ls", text);
}
