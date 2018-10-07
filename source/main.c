#include <wchar.h>
#include <locale.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <ncursesw/curses.h>

#include "mob.h"
#include "main.h"
#include "dungeon.h"
#include "path.h"

const char* HELP = "--help";
const char* SAVE = "--save";
const char* LOAD = "--load";
const char* MOBS = "--nummon";
const char* LAME = "--lame";
const char* ALL = "--all";


static volatile int running = 1;
static void sig() {
	fflush(stdout);
	running = 0;
}

static void help(char* message, char* command, Error error ) {
	wprintf(L"%s: ", command);
	wprintf(L"%s\n", message);
	wprintf(
		L"Remember, please see the readme if anything looks weird!\n"
		L"Adventure Commander Help:\n"
		L"[--help] Show this\n"
		L"[--lame] Disable emoji support (needed for older terms)\n"
		L"[--load] Load a file from ~/.rlg327/dungeon\n"
		L"[--save] Write a dungeon to ~/.rlg327/dungeon\n"
		L"[--all] Show all moves instead of just PC moves\n"
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
	int all = 0;
	int emoji = 1;

	//Parse arguments
	for (int arg = 1; arg < argc; arg++) {
		if (strlen(argv[arg]) < 3 || argv[arg][0] != '-' || argv[arg][1] != '-') {
			//Check length
			help("Bad argument", argv[arg], ARGUMENT_NO_DASH);
		} else {
			//Check if correct argument
			if (strcmp(HELP, argv[arg]) == 0) {
				help("Listing Commands", argv[arg], 0);
			} else if (strcmp(LAME, argv[arg]) == 0) {
				emoji = 0;
			} else if (strcmp(SAVE, argv[arg]) == 0) {
				save = 1;
			} else if (strcmp(LOAD, argv[arg]) == 0) {
				load = 1;
			} else if (strcmp(ALL, argv[arg]) == 0) {
				all = 1;
			} else if (strcmp(MOBS, argv[arg]) == 0 && require(&arg, argc, argv[arg])) {
				mobs = atoi(argv[arg]); // NOLINT(cert-err34-c)
				if (mobs < 1) help("Must be between 1-100", (char*)MOBS, ARGUMENT_OOB);
				if (mobs > 100) help("Must be between 1-100", (char*)MOBS, ARGUMENT_OOB);
			} else {
				help("Unknown argument", argv[arg], ARGUMENT_UNKNOWN);
			}
		}
	}

	//The base dungeon
	Dungeon dungeon;

	//Load dungeon from file
	if (load) {
		FILE* file = get("rb");
		dungeon = dungeonLoad(file, mobs, emoji);
		fclose(file);
	} else {
		dungeon = dungeonGenerate(DUNGEON_DIM, mobs, emoji);
	}

	//Business executed
	WINDOW* base = initscr();
	cbreak();
	noecho();
	wclear(base);
	curs_set(0);

	//Economics established
	setText(dungeon, &dungeon.status, L"Nothing has happened yet!");
	setText(dungeon, &dungeon.line1, L"Something will go here one day!");
	setText(dungeon, &dungeon.line2, L"Waiting for other players...");
	pathCreate(&dungeon);
	Node* turn = mobCreateQueue(&dungeon);
	while (running && dungeon.player.hp > 0 && mobAliveCount(dungeon)) {
		int priority = queuePeekPriority(&turn);
		Mob* mob = queuePeek(&turn).mob;
		queuePop(&turn);
		mobTick(mob, &dungeon);
		queuePushSub(&turn, (NodeData){.mob=mob}, priority + 1000/mob->speed, mob->order);
		if (all || mob->skills & SKILL_PC) {
			dungeonPrint(base, dungeon);
			usleep(200000);
		}
	}

	//If we didn't quit early, show something
	if (running) {
		if (dungeon.player.hp == 0) {
			setText(dungeon, &dungeon.status, L"You died! Better luck next time!");
		} else {
			setText(dungeon, &dungeon.status, L"CONGLATURATION !!!");
		}
	}

	//Show the dungeon once more before exiting.
	dungeonPrint(base, dungeon);

	//Save dungeon to file.
	if (save) {
		FILE* file = get("wb");
		dungeonSave(dungeon, file);
		fclose(file);
	}

	queueDestroy(&turn);
	dungeonDestroy(&dungeon);
	endwin();
}
