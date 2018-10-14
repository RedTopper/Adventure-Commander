#define _XOPEN_SOURCE_EXTENDED

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
#include "stack.h"

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
	StackNode* history = NULL;
	StackNode* future = NULL;
	int floor = 0;

	//Load dungeon from file
	if (load) {
		FILE* file = get("rb");
		dungeon = dungeonLoad(file, mobs, emoji, floor);
		fclose(file);
	} else {
		dungeon = dungeonGenerate(DUNGEON_DIM, mobs, emoji, floor);
	}

	//Business executed
	WINDOW* base = initscr();
	cbreak();
	noecho();
	wclear(base);
	curs_set(0);

	//Economics established
	while (running && dungeon.player->hp > 0 && mobAliveCount(dungeon)) {
		int priority = queuePeekPriority(&dungeon.turn);
		Mob* mob = queuePeek(&dungeon.turn).mob;
		queuePop(&dungeon.turn);
		Action action = mobTick(mob, &dungeon, base);
		queuePushSub(&dungeon.turn, (QueueNodeData){.mob = mob}, priority + 1000/mob->speed, mob->order);

		if (action == ACTION_QUIT) {
			running = 0;
			break;
		}

		if (action == ACTION_DOWN && !stackEmpty(&history)) {
			stackPush(&future, (StackNodeData){.dungeon = dungeon});
			dungeon = stackPeek(&history).dungeon;
			stackPop(&history);
		}

		if (action == ACTION_UP) {
			stackPush(&history, (StackNodeData){.dungeon = dungeon});
			if (stackEmpty(&future)) {
				dungeon = dungeonGenerate(DUNGEON_DIM, mobs, emoji, ++floor);
			} else {
				dungeon = stackPeek(&future).dungeon;
				stackPop(&future);
			}
		}

		if (all || mob->skills & SKILL_PC) {
			dungeonPrint(base, dungeon);
			if (all) usleep(500000);
		}
	}

	//Some status messages
	size_t len = (size_t) (dungeon.dim.x) + 1;
	if (running && dungeon.player->hp == 0) {
		swprintf(dungeon.status, len, L"You died! Better luck next time! (Press any key)");
	} else if (running && mobAliveCount(dungeon) == 0) {
		swprintf(dungeon.status, len, L"CONGLATURATION !!! (Press any key)");
	} else {
		swprintf(dungeon.status, len, L"See you later! (Press any key)");
	}

	//Show the dungeon once more before exiting.
	redrawwin(base);
	dungeonPrint(base, dungeon);
	getch();

	//Save dungeon to file.
	if (save) {
		FILE* file = get("wb");
		dungeonSave(dungeon, file);
		fclose(file);
	}

	//Free the stacks
	while(!stackEmpty(&history)) {
		Dungeon temp = stackPeek(&history).dungeon;
		dungeonDestroy(&temp);
		stackPop(&history);
	}

	//Free the future too
	while(!stackEmpty(&future)) {
		Dungeon temp = stackPeek(&future).dungeon;
		dungeonDestroy(&temp);
		stackPop(&future);
	}

	//And free the current dungeon
	dungeonDestroy(&dungeon);

	//That's all folks.
	endwin();
}
