#include <iomanip>
#include "dungeon.hpp"

Player::Player(Dungeon *dungeon, WINDOW *window) : Mob(
	dungeon,
	nullptr,
	DEF_COLOR,
	DEF_ORDER,
	DEF_SKILLS,
	DEF_SPEED,
	DEF_HP,
	Dice(1, 1, 1),
	"Player",
	"\U0001F464",
	"@",
	vector<string>()
) {
	this->base = window;
	this->action = NONE;
	this->turn = 0;
	this->dam = Dice(0, 1, 4);
}

string Player::relative(const Mob& other) {
	stringstream str;
	str
		<< setw(16) << other.getName()
		<< " (" << other.getSymbolAlt() << "/" << other.getSymbol() << ")"
		<< (other.isAlive() ? " is " : " was killed ")
		<< abs(pos.y - other.getPos().y)
		<< (other.getPos().y > pos.y ?  " south " : " north ")
		<< "and "
		<< abs(pos.x - other.getPos().x)
		<< (other.getPos().x < pos.x ? " west." : " east.");
	return str.str();
}

string Player::item(const Object& object, int index) {
	stringstream str;
	str
		<< index << ": "
		<< "(" << object.getSymbolAlt() << "/" << object.getSymbol() << ") "
		<< object.getName();
	return str.str();
}

bool Player::tickScroll(int ch, uint &offset, const string& title, const vector<string>& constLines) {
	vector<string> lines = constLines;
	if (lines.empty()) lines.emplace_back("We've come up empty!");

	//Clear screen and figure out characters
	int max = LINES - 2;
	werase(base);
	switch (ch) {
		case KEY_UP:
			if (offset == 0) beep(); //FALLTHROUGH
		case KEY_RESIZE:
			if (offset > 0) offset--;
			break;
		case KEY_DOWN:
			if (offset + max < lines.size()) {
				offset++;
			} else {
				beep();
			}
			break;
		case 27: //ESC
		case 'q':
		case 'Q':
			return false;
		default:
			break;
	}

	//Show screen
	mvwaddstr(base, 0, 0, (title + " Press 'q' or 'ESC' to exit.").c_str());
	for (int line = 0; line < max; line++) {
		string text = line + offset < lines.size() ? lines[line + offset] : "~";
		mvwaddstr(base, line + 1, 0, text.c_str());
	}

	//Print end marker if needed
	if (offset + max >= lines.size()) {
		wattron(base, WA_STANDOUT);
		mvwaddstr(base, LINES - 1, 0, "(END)");
		wattroff(base, WA_STANDOUT);
	} else {
		mvwaddstr(base, LINES - 1, 0, ":");
	}

	//Print and get new character
	wrefresh(base);
	return true;
}

bool Player::tickTarget(const int ch, Point& dest) {
	switch (ch) {
		case '7':
		case 'y':
			dest += Point(-1,-1);
			break;
		case '8':
		case 'k':
			dest += Point(0,-1);
			break;
		case '9':
		case 'u':
			dest += Point(1,-1);
			break;
		case '6':
		case 'l':
			dest += Point(1,0);
			break;
		case '3':
		case 'n':
			dest += Point(1,1);
			break;
		case '2':
		case 'j':
			dest += Point(0,1);
			break;
		case '1':
		case 'b':
			dest += Point(-1,1);
			break;
		case '4':
		case 'h':
			dest += Point(-1,0);
			break;
		case 27: //ESC
		case 'q':
			return false;
		case 'r':
			dest = Point((rand() % (dungeon->getDim().x - 2)) + 1, (rand() % (dungeon->getDim().y - 2)) + 1);
			//FALLTHROUGH
		case 'g':
			dungeon->status = "Woosh!";
			dungeon->getTile(dest).type = Tile::HALL;
			move(dest);
			return false;
		default:
			break;
	}

	if (dest.x < 1) dest.x = 1;
	if (dest.y < 1) dest.y = 1;
	if (dest.x >= dungeon->getDim().x - 1)  dest.x = dungeon->getDim().x - 2;
	if (dest.y >= dungeon->getDim().y - 1)  dest.y = dungeon->getDim().y - 2;

	dungeon->print(base);

	//Move the destination down one because of the first status line
	mvwaddstr(base, dest.y + 1, dest.x, "\u00D7");

	return true;
}

void Player::tickInput() {
	dungeon->updateFoggy();
	dungeon->print(base);
	action = NONE;
	Movement res = IDLE;
	uint32_t offset = 0;
	Point dest = pos;
	vector<string> lines;
	int ch = getch();
	switch (ch) {
		case KEY_HOME:
		case '7':
		case 'y':
			res = move(Point(pos) += Point(-1,-1));
			break;
		case KEY_UP:
		case '8':
		case 'k':
			res = move(Point(pos) += Point(0,-1));
			break;
		case KEY_PPAGE:
		case '9':
		case 'u':
			res = move(Point(pos) += Point(1,-1));
			break;
		case KEY_RIGHT:
		case '6':
		case 'l':
			res = move(Point(pos) += Point(1,0));
			break;
		case KEY_NPAGE:
		case '3':
		case 'n':
			res = move(Point(pos) += Point(1,1));
			break;
		case KEY_DOWN:
		case '2':
		case 'j':
			res = move(Point(pos) += Point(0,1));
			break;
		case KEY_END:
		case '1':
		case 'b':
			res = move(Point(pos) += Point(-1,1));
			break;
		case KEY_LEFT:
		case '4':
		case 'h':
			res = move(Point(pos) += Point(-1,0));
			break;
		case KEY_B2:
		case '.':
		case '5':
		case ' ':
			action = STALL;
			break;
		case '>':
			if (isOnEntity(STAIRS_DOWN)) {
				action = DOWN;
			} else {
				dungeon->status = "You are not on a down staircase!";
			}
			break;
		case '<':
			if (isOnEntity(STAIRS_UP)) {
				action = UP;
			} else {
				dungeon->status = "You are not on an up staircase!";
			}
			break;
		case 'Q':
			action = QUIT;
			break;
		case 'q':
			dungeon->status = "If you really want to quit, try 'shift-q' instead.";
			break;
		case 'm':
			//keep ticking and getting characters until tick returns false
			for (const auto& m : dungeon->getMobs()) lines.push_back(relative(*m));
			while(tickScroll(ch, offset, "Monster list.", lines)) ch = getch();
			break;
		case '\t':
			while(tickScroll(ch, offset, "In game manual.", getHelp())) ch = getch();
			break;
		case 'i':
			for (const auto& o : inventory) lines.push_back(item(*o, offset++));
			offset = 0;
			while(tickScroll(ch, offset, "Your Inventory.", lines)) ch = getch();
			break;
		case 'g':
			ch = 0;
			dungeon->status = "You cheater! You entered teleport mode!";
			while(tickTarget(ch, dest)) ch = getch();
			break;
		case 'f':
			dungeon->status = "You cheater! You toggled the fog!";
			dungeon->setDisplay(dungeon->getDisplay() == Dungeon::FOGGY ? Dungeon::NORMAL : Dungeon::FOGGY);
			break;
		case 'D':
			dungeon->status = "You cheater! You switched to path-find distance!";
			dungeon->setDisplay(Dungeon::DISTANCE);
			break;
		case 'T':
			dungeon->status = "You cheater! You switched to path-find dig!";
			dungeon->setDisplay(Dungeon::TUNNEL);
			break;
		case ',':
			pickUpObject();
			break;
		default:
			dungeon->status = to_string(ch) + " is invalid!";
	}

	//Move the player character
	if (res == FAILURE) dungeon->status =  "I can't dig through that!";
	if (res == SUCCESS) {
		action = MOVE;
		dungeon->recalculate();
	}
}

void Player::tick() {
	action = NONE;
	dungeon->status = "It's your turn! (TAB for help)";
	while (action == NONE) {
		tickInput();
	}

	attack();
}

Mob::Pickup Player::pickUpObject() {
	Pickup pickup = Mob::pickUpObject();
	switch(pickup) {
		case NOTHING:
			dungeon->status = "Nothing to pick up";
			break;
		case SPACE:
			dungeon->status = "You are holding too many items";
			break;
		case WEIGHT:
			dungeon->status = "The item beneath you is too heavy to carry";
			break;
		case ADD:
			dungeon->status = "You picked up " + inventory.back()->getName() + " (" + inventory.back()->getSymbol() + ")";
			break;
	}

	return pickup;
}

const vector<string> Player::getHelp() {
	vector<string> help = {
		"ADVENTURE COMMANDER(6)",
		"",
		"NAME",
		"\tAdventure Commander - A text based rogue-like game for COM S 327",
		"",
		"CONTROLS",
		"\tCritical Controls:",
		"\tQ",
		"\t\tQuit the game gracefully",
		"\t",
		"\tMovement Controls:",
		"\t7, y, KEY_HOME",
		"\t\tMove/Attack up left",
		"\t8, k, KEY_UP",
		"\t\tMove/Attack up",
		"\t9, u, KEY_PPAGE",
		"\t\tMove/Attack up right",
		"\t6, l, KEY_RIGHT",
		"\t\tMove/Attack right",
		"\t3, n, KEY_NPAGE",
		"\t\tMove/Attack down right",
		"\t2, j, KEY_DOWN",
		"\t\tMove/Attack down",
		"\t1, b, KEY_END",
		"\t\tMove/Attack down left",
		"\t4, h, KEY_LEFT",
		"\t\tMove/Attack left",
		"\t5, ., KEY_B2, SPACE BAR",
		"\t\tWait a turn",
		"\t",
		"\tInteraction Controls:",
		"\t>",
		"\t\tGo down a staircase",
		"\t<",
		"\t\tGo up a staircase",
		"\t,",
		"\t\tPick up an item off the ground",
		"\t",
		"\tCharacter Controls:",
		"\ti",
		"\t\tShow your inventory",
		"\t",
		"\tDebug Controls:",
		"\tg",
		"\t\tGoto/Teleport - r for random, g for finish",
		"\tT",
		"\t\tShow the tunneling path-finding map",
		"\tD",
		"\t\tShow the distance path-finding map",
		"\tf",
		"\t\tToggle fog of war",
		"\tm",
		"\t\tShow a list of monsters on the map",
		"",
		"DESCRIPTION",
		"\tThis text game uses NCurses and C++ to implement a rogue-like game.",
		"\tYou can play the game via command line on most linux systems. To compile",
		"\tfrom source, you can use the Makefile provided in the repository root.",
		"\tThere is an additional CMakeLists.txt provided if you would like to use",
		"\tCMake to build the program instead.",
		"\t",
		"\tThis game uses Emoji (if enabled, see OPTIONS) and wide characters to get",
		"\tthe most out of your terminal emulator. This game will probably not work",
		"\tin any non UTF-8 conforming terminal (such as real terminals, ironically).",
		"\t",
		"\tHave fun killing monsters and conquering dungeons in ADVENTURE COMMANDER!",
		"",
		"OPTIONS",
	};

	for(const auto& str : getOptions()) help.push_back("\t" + str);

	vector<string> post = {
		"",
		"AUTHOR",
		"\tMade with <3 by Aaron Walter <ajwalter@iastate.edu>",
		"\t(Thanks for playing!)",
		"",
		"ADVENTURE COMMANDER(6)"
	};

	help.insert(help.end(), post.begin(), post.end());

	return help;
}
