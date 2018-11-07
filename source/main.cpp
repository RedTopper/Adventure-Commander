#include <iostream>
#include <cstring>
#include <locale>
#include <string>
#include <stack>
#include <fstream>
#include <chrono>
#include <thread>

#include "main.hpp"
#include "dungeon.hpp"
#include "stream/smob.hpp"
#include "stream/sentity.hpp"
#include "stream/dice.hpp"

using namespace std;

const char* HELP = "--help";
const char* SAVE = "--save";
const char* LOAD = "--load";
const char* MOBS = "--nummon";
const char* LAME = "--lame";
const char* HOME = "--home";
const char* PARSE = "--parse";
const char* ALL = "--all";

static void help(const string& message, const string& command, Error error) {
	cout << command << ": " << message << endl;
	cout << "Remember, please see the readme if anything looks weird!" << endl;
	cout << "Adventure Commander Help:" << endl;
	cout << "[--help] Show this" << endl;
	cout << "[--lame] Disable emoji support (needed for older terms)" << endl;
	cout << "[--load] Load a file from /dungeon" << endl;
	cout << "[--save] Write a dungeon to /dungeon" << endl;
	cout << "[--all] Show all moves instead of just PC moves" << endl;
	cout << "[--parse] Attempt to parse /monster_desc.txt" << endl;
	cout << "[--home] Access files from the ~/.rlg327 directory instead of /resource" << endl;
	cout << "[--nummon] <count> Number of monsters to generate" << endl;
	exit(error);
}

static bool require(int& on, int count, const string& command) {
	if (on < count - 1) {
		on++;
	} else {
		help("This param requires another argument", command, ARGUMENT_REQ_PARAM);
	}

	return true;
}

static fstream get(ios_base::openmode mode, const string& name, bool home) {
	string path;
	if (home) {
		path = getenv("HOME");
		path += "/.rlg327/";
	} else {
		path = "./resource/";
	}

	//Add filename and load from file
	path += name;
	fstream fs;
	fs.open (path, fstream::binary | mode);
	if (!fs.is_open() || !fs) {
		string message = "Failed to open file";
		if (!home) message += "\nUse --home to load from home directory instead.";
		help(message, path, FILE_READ_GONE);
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

template <class T>
vector<T> createClass(const string& filename, const string& header, bool home){
	string line;
	vector<T> factory;
	fstream desc = get(fstream::in, filename, home);
	getline(desc, line);
	if (line.find(header) == string::npos) exit(FILE_READ_BAD_HEAD);
	while (!!desc) {
		T obj;
		desc >> obj;
		if(obj.isValid()) factory.push_back(obj);
	}

	desc.close();
	return factory;
}

int main(int argc, char** argv) {
	setlocale(LC_ALL, "en_US.UTF-8");
	srand((unsigned int) time(nullptr));

	int mobs = 10;
	bool save = false;
	bool load = false;
	bool all = false;
	bool emoji = true;
	bool parse = false;
	bool home = false;

	//Parse arguments
	for (int argi = 1; argi < argc; argi++) {
		string arg = argv[argi];
		if (arg.length() < 3 || arg[0] != '-' || arg[1] != '-') {
			//Check length
			help("Bad argument", arg, ARGUMENT_NO_DASH);
		} else {
			//Check if correct argument
			if (HELP == arg) {
				help("Listing Commands", arg, FINE);
			} else if (arg == LAME) {
				emoji = false;
			} else if (arg == SAVE) {
				save = true;
			} else if (arg == LOAD) {
				load = true;
			} else if (arg == ALL) {
				all = true;
			} else if (arg == HOME) {
				home = true;
			} else if (arg == PARSE) {
				parse = true;
			} else if (arg == MOBS && require(argi, argc, arg)) {
				mobs = atoi(argv[argi]); // NOLINT(cert-err34-c)
				if (mobs < 1) help("Must be between 1-100", MOBS, ARGUMENT_OOB);
				if (mobs > 100) help("Must be between 1-100", MOBS, ARGUMENT_OOB);
			} else {
				help("Unknown argument", arg, ARGUMENT_UNKNOWN);
			}
		}
	}

	//Load both factory files
	vector<SMob> factoryMobs = createClass<SMob>("monster_desc.txt", "RLG327 MONSTER DESCRIPTION 1", home);
	vector<SEntity> factoryObjects = createClass<SEntity>("object_desc.txt", "RLG327 OBJECT DESCRIPTION 1", home);

	//Still output the files loaded if needed
	if (parse) {
		for(auto& o : factoryMobs) cout << o;
		for(auto& o : factoryObjects) cout << o;
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
		fstream file = get(fstream::in, "dungeon", home);
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
			dungeon->status = "It's your turn!";
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
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}
	}

	//Some status messages
	if (!dungeon->getPlayer()->isAlive()) {
		dungeon->status = "You died! Better luck next time! (Press any key)";
	} else if (dungeon->alive() == 0) {
		dungeon->status = "CONGLATURATION !!! (Press any key)";
	} else {
		dungeon->status = "See you later! (Press any key)";
	}

	//Show the dungeon once more before exiting.
	redrawwin(base);
	dungeon->setFoggy(false);
	dungeon->print(base);
	getch();

	//Save dungeon to file.
	if (save) {
		fstream file = get(fstream::out, "dungeon", home);
		dungeon->save(file);
		file.close();
	}

	//That's all folks.
	endwin();
}
