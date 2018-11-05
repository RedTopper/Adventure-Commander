#define _XOPEN_SOURCE_EXTENDED

#include <iostream>
#include <cstring>
#include <locale>
#include <codecvt>
#include <string>
#include <stack>
#include <fstream>
#include <unistd.h>

#include "stream/dice.hpp"
#include "main.hpp"
#include "dungeon.hpp"
#include "stream/monster.hpp"

using namespace std;

const char* HELP = "--help";
const char* SAVE = "--save";
const char* LOAD = "--load";
const char* MOBS = "--nummon";
const char* LAME = "--lame";
const char* PARSE = "--parse";
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
	wcout << L"[--parse] Attempt to parse ~/.rlg327/monster_desc.txt" << endl;
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

static fstream get(ios_base::openmode mode, const string& name) {
	string dungeon = getenv("HOME");
	dungeon += "/.rlg327/";
	dungeon += name;

	//Load dungeon from file
	fstream fs;
	fs.open (dungeon, fstream::binary | mode);
	if (!fs.is_open() || !fs) {
		help(L"Failed to open file", dungeon, FILE_READ_GONE);
	}

	return fs;
}

int skewBetweenRange(const int skew, const int low, const int high) {
	int value = low;
	while (rand() % skew && value < high) value++;
	return value;
}

string &ltrim(string &str, const string &chars) {
	str.erase(0, str.find_first_not_of(chars));
	return str;
}

string &rtrim(string &str, const string &chars) {
	str.erase(str.find_last_not_of(chars) + 1);
	return str;
}

string &trim(string &str, const string &chars) {
	return ltrim(rtrim(str, chars), chars);
}

int main(int argc, char** argv) {
	setlocale(LC_ALL, "");
	srand((unsigned int) time(nullptr));

	bool save = false;
	bool load = false;
	int mobs = 10;
	bool all = false;
	bool emoji = true;
	bool parse = false;

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
			} else if (arg == PARSE) {
				parse = true;
			} else if (arg == MOBS && require(argi, argc, arg)) {
				mobs = atoi(argv[argi]); // NOLINT(cert-err34-c)
				if (mobs < 1) help(L"Must be between 1-100", MOBS, ARGUMENT_OOB);
				if (mobs > 100) help(L"Must be between 1-100", MOBS, ARGUMENT_OOB);
			} else {
				help(L"Unknown argument", arg, ARGUMENT_UNKNOWN);
			}
		}
	}

	if (parse) {
		fstream file = get(fstream::in, "monster_desc.txt");
		string line;
		getline(file, line);
		if (line != "RLG327 MONSTER DESCRIPTION 1") exit(FILE_READ_BAD_HEAD);
		while (!!file) {
			StreamMob mob;
			file >> mob;
			if(mob.isValid()) cout << mob;
		}

		return 0;
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
		fstream file = get(fstream::in, "dungeon");
		dungeon = make_shared<Dungeon>(base, file, mobs, emoji);
		file.close();
	} else {
		dungeon = make_shared<Dungeon>(base, DUNGEON_DIM, mobs, 0, emoji);
	}

	//Economics established
	while (dungeon->getPlayer()->isAlive() > 0 && dungeon->alive()) {
		shared_ptr<Mob> mob = dungeon->getCurrentTurn();
		shared_ptr<Player> player;
		if (player = dynamic_pointer_cast<Player>(mob)) {
			//If it's a player, keep ticking until something interesting happens
			dungeon->status = L"It's your turn!";
			Player::Action action = Player::NONE;
			while(action == Player::NONE) {
				player->tick();
				action = player->getAction();
			}

			if (action == Player::QUIT) break;
			if (action == Player::DOWN && !history.empty()) {
				future.push(dungeon);
				dungeon = history.top();
				history.pop();
			}

			if (action == Player::UP) {
				history.push(dungeon);
				if (future.empty()) {
					dungeon = make_shared<Dungeon>(base, DUNGEON_DIM, mobs, ++floor, emoji);
				} else {
					dungeon = future.top();
					future.pop();
				}
			}
		} else {
			mob->tick();
		}

		dungeon->rotate();

		if (all) {
			dungeon->print(base);
			usleep(500000);
		}
	}

	//Some status messages
	if (!dungeon->getPlayer()->isAlive()) {
		dungeon->status = L"You died! Better luck next time! (Press any key)";
	} else if (dungeon->alive() == 0) {
		dungeon->status = L"CONGLATURATION !!! (Press any key)";
	} else {
		dungeon->status = L"See you later! (Press any key)";
	}

	//Show the dungeon once more before exiting.
	redrawwin(base);
	dungeon->setFoggy(false);
	dungeon->print(base);
	getch();

	//Save dungeon to file.
	if (save) {
		fstream file = get(fstream::out, "dungeon");
		dungeon->save(file);
		file.close();
	}

	//That's all folks.
	endwin();
}
