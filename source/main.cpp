#define _XOPEN_SOURCE_EXTENDED

#include <iostream>
#include <cstring>
#include <locale>
#include <codecvt>
#include <string>
#include <stack>
#include <fstream>

#include "main.hpp"
#include "dungeon.hpp"

using namespace std;

const char* HELP = "--help";
const char* SAVE = "--save";
const char* LOAD = "--load";
const char* MOBS = "--nummon";
const char* LAME = "--lame";
const char* ALL = "--all";

static void help(const wstring& message, const string& command, Error error) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wc;
	wcout << wc.from_bytes(command) << ": " << message << endl;
	wcout << L"Remember, please see the readme if anything looks weird!" << endl;
	wcout << L"Adventure Commander Help:" << endl;
	wcout << L"[--help] Show this" << endl;
	wcout << L"[--lame] Disable emoji support (needed for older terms)" << endl;
	wcout << L"[--load] Load a file from ~/.rlg327/dungeon" << endl;
	wcout << L"[--save] Write a dungeon to ~/.rlg327/dungeon" << endl;
	wcout << L"[--all] Show all moves instead of just PC moves" << endl;
	wcout << L"[--nummon] <count> Number of monsters to generate" << endl;
	exit(error);
}

static bool require(int& on, int count, const string& command) {
	if (on < count - 1) {
		on++;
	} else {
		help(L"This param requires another argument", command, ARGUMENT_REQ_PARAM);
	}

	return true;
}

static fstream get(ios_base::openmode mode) {
	string dungeon = getenv("HOME");
	dungeon += "/.rlg327/dungeon";

	//Load dungeon from file
	fstream fs;
	fs.open (dungeon, fstream::binary | mode);
	if (!fs) {
		help(L"Failed to open file", dungeon, FILE_READ_GONE);
	}

	return fs;
}

int skewBetweenRange(const int skew, const int low, const int high) {
	int value = low;
	while (rand() % skew && value < high) value++;
	return value;
}

int main(int argc, char** argv) {
	setlocale(LC_ALL, "");
	srand((unsigned int) time(nullptr));

	bool save = false;
	bool load = false;
	int mobs = 10;
	bool all = false;
	bool emoji = true;

	//Parse arguments
	for (int argi = 1; argi < argc; argi++) {
		string arg = argv[argi];
		if (arg.length() < 3 || arg[0] != '-' || arg[1] != '-') {
			//Check length
			help(L"Bad argument", arg, ARGUMENT_NO_DASH);
		} else {
			//Check if correct argument
			if (HELP == arg) {
				help(L"Listing Commands", arg, FINE);
			} else if (arg == LAME) {
				emoji = false;
			} else if (arg == SAVE) {
				save = true;
			} else if (arg == LOAD) {
				load = true;
			} else if (arg == ALL) {
				all = true;
			} else if (arg == MOBS && require(argi, argc, arg)) {
				mobs = atoi(argv[argi]); // NOLINT(cert-err34-c)
				if (mobs < 1) help(L"Must be between 1-100", MOBS, ARGUMENT_OOB);
				if (mobs > 100) help(L"Must be between 1-100", MOBS, ARGUMENT_OOB);
			} else {
				help(L"Unknown argument", arg, ARGUMENT_UNKNOWN);
			}
		}
	}

	//The base dungeon
	shared_ptr<Dungeon> dungeon;
	stack<shared_ptr<Dungeon>> history;
	stack<shared_ptr<Dungeon>> future;
	int floor = 0;

	//Business executed
	WINDOW* base = initscr();
	keypad(base, TRUE);
	cbreak();
	noecho();
	wclear(base);
	curs_set(0);

	//Load dungeon from file
	if (load) {
		fstream file = get(fstream::in);
		dungeon = make_shared<Dungeon>(new Dungeon(base, file, mobs, emoji);
		file.close();
	} else {
		dungeon = make_shared<Dungeon>(new Dungeon(base, DUNGEON_DIM, mobs, 0, emoji));
	}

	//Economics established
	while (dungeon->getPlayer().isAlive() > 0 && dungeon->alive()) {
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
