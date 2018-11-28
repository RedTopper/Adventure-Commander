#include <iostream>
#include <locale>
#include <stack>
#include <fstream>

#include "twist.hpp"
#include "main.hpp"
#include "dungeon.hpp"

using namespace std;

const char* HELP = "--help";
const char* SAVE = "--save";
const char* LOAD = "--load";
const char* MOBS = "--nummon";
const char* LAME = "--lame";
const char* HOME = "--home";
const char* PARSE = "--parse";
const char* ALL = "--all";

vector<string> getOptions() {
	vector<string> options = {
		"[--help] Show this",
		"[--lame] Disable emoji support (needed for older terms)",
		"[--load] Load a file from /dungeon",
		"[--save] Write a dungeon to /dungeon",
		"[--all] Show all moves instead of just PC moves",
		"[--parse] Attempt to parse /monster_desc.txt",
		"[--home] Access files from the ~/.rlg327 directory instead of /resource",
		"[--nummon] <count> Number of monsters to generate",
	};

	return options;
}

static void help(const string& message, const string& command, Error error) {
	cout << command << ": " << message << endl;
	cout << "Remember, please see the readme if anything looks weird!" << endl;
	cout << "Adventure Commander Help:" << endl;
	for(const auto& str : getOptions()) cout << str << endl;
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

Mob::Color getRandomColor(int colors) {
	if (!colors) return Mob::Color::WHITE;

	//Count the number of bits set in colors
	int bits = colors;
	int count = 0;
	while (bits) {
		count += bits & 1;
		bits >>= 1;
	}

	//Get a random number from 1 - set bits
	int out = (rand() % count) + 1;
	int color = 1;

	//Check if we hit a bit
	while (out) {
		if (color & colors) out--;
		color <<= 1;
	}

	return static_cast<Mob::Color>(color >> 1);
}

template <class T>
vector<T> loadFactory(const string &filename, const string &header, bool home) {
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
			help("Bad argument", arg, ARGUMENT_NO_DASH);
		} else if (HELP == arg) {
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

	//Load both factory files
	vector<FMob> factoryMobs = loadFactory<FMob>("monster_desc.txt", "RLG327 MONSTER DESCRIPTION 1", home);
	vector<FObject> factoryObjects = loadFactory<FObject>("object_desc.txt", "RLG327 OBJECT DESCRIPTION 1", home);

	if (factoryMobs.empty()) help("Parsed file was empty. Try using --parse to debug.", "Mobs", FILE_READ_BAD);
	if (factoryObjects.empty()) help("Parsed file was empty. Try using --parse to debug.", "Objects", FILE_READ_BAD);

	//Still output the files loaded if needed
	if (parse) {
		for(auto& o : factoryMobs) cout << o;
		for(auto& o : factoryObjects) cout << o;
		return 0;
	}

	//The base dungeon
	shared_ptr<Player> player;
	shared_ptr<Dungeon> dungeon;
	stack<shared_ptr<Dungeon>> history;
	stack<shared_ptr<Dungeon>> future;
	int floor = 0;

	//Business executed
	WINDOW* base = initscr();
	if (has_colors()) {
		use_default_colors();
		start_color();
		init_pair(Entity::RED, COLOR_RED, -1);
		init_pair(Entity::GREEN, COLOR_GREEN, -1);
		init_pair(Entity::BLUE, COLOR_BLUE, -1);
		init_pair(Entity::CYAN, COLOR_CYAN, -1);
		init_pair(Entity::YELLOW, COLOR_YELLOW, -1);
		init_pair(Entity::MAGENTA, COLOR_MAGENTA, -1);
		init_pair(Entity::WHITE, COLOR_WHITE, -1);
		init_pair(Entity::BLACK, 238, -1);
	}

	keypad(base, TRUE);
	cbreak();
	noecho();
	wclear(base);
	curs_set(0);

	//Load dungeon from file
	if (load) {
		fstream file = get(fstream::in, "dungeon", home);
		dungeon = make_shared<Dungeon>(file);
		dungeon->finalize(base, factoryMobs, factoryObjects, mobs, 0, emoji, player);
		file.close();
	} else {
		dungeon = make_shared<Dungeon>(DUNGEON_DIM);
		dungeon->finalize(base, factoryMobs, factoryObjects, mobs, 0, emoji, player);
	}

	//Economics established
	while (dungeon->getPlayer()->isAlive() > 0 && dungeon->alive()) {
		shared_ptr<Mob> mob = dungeon->getCurrentTurn();

		//Tick and end turn
		mob->tick();
		dungeon->rotate();

		Mob::Action action = mob->getAction();
		if (action == Player::AC_QUIT) break;
		if (action == Player::AC_DOWN && !history.empty()) {
			//Take snapshot before leaving
			dungeon->snapshotTake();
			future.push(dungeon);

			//Going down, already existing dungeon
			dungeon = history.top();
			dungeon->snapshotRestore();
			history.pop();
		}

		if (action == Player::AC_UP) {
			//Take snapshot before leaving
			dungeon->snapshotTake();
			history.push(dungeon);

			if (future.empty()) {
				//Going up, create new dungeon
				dungeon = make_shared<Dungeon>(DUNGEON_DIM);
				dungeon->finalize(base, factoryMobs, factoryObjects, mobs, ++floor, emoji, player);
			} else {
				//Going up, already existing dungeon
				dungeon = future.top();
				dungeon->snapshotRestore();
				future.pop();
			}
		}

		if (all) {
			dungeon->print(base);
			getch();
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
	dungeon->setDisplay(Dungeon::NORMAL);
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
