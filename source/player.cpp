#include <iomanip>
#include <player.hpp>


#include "player.hpp"
#include "dungeon.hpp"

static int center(const string& str) {
	return (COLS / 2) - ((int)str.size() / 2);
}

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
	this->action = AC_NONE;
	this->turn = 0;
	this->dam = Dice(0, 1, 4);
}

string Player::displayMob(const Mob &other) {
	stringstream str;
	str
		<< "(" << other.getSymbol() << ") "
		<< setw(16) << other.getName()
		<< (other.isAlive() ? " is " : " was killed ")
		<< abs(pos.y - other.getPos().y)
		<< (other.getPos().y > pos.y ?  " south " : " north ")
		<< "and "
		<< abs(pos.x - other.getPos().x)
		<< (other.getPos().x < pos.x ? " west." : " east.");
	return str.str();
}

string Player::displayObject(const Object &object) {
	stringstream str;
	str
		<< "("
		<< object.getSymbol()
		<< " "
		<< FObject::fromType(object.getTypes())
		<< ") "
		<< object.getName();
	return str.str();
}

void Player::list(const deque<shared_ptr<Object>>& objects, const string& title, int start, void (Player::*action)(int)) {
	int ch = 0;
	char letter = (char)start;
	uint32_t offset = 0;

	//Construct a vector with each item on a line
	vector<string> lines;
	lines.reserve(objects.size());
	for (const auto& o : objects) lines.push_back(string() + letter++ + ": " + displayObject(*o));

	//List the items
	while(tickScroll(ch, offset, title, lines)) {
		ch = getch();

		//If the letter picked was between the start letter and the end letter
		if (ch >= start && ch < (uint32_t)start + objects.size()) {

			//Call the given function
			(this->*action)(ch - start);
			break;
		}
	}
}

void Player::expunge(int index) {
	if (index < 0 || (uint32_t)index >= inventory.size()) {
		dungeon->status = "Cannot expunge - invalid inventory selection.";
		return;
	}

	auto item = inventory[index];

	bool remove = choice({
		"HOLD UP!",
		"You are about to DESTROY this item FOREVER:",
		"(a long time)",
		"",
		displayObject(*item),
		"",
		"Are you REALLY sure you want to do that?"
	});

	if (remove) {
		inventory.erase(inventory.begin() + index);
		dungeon->status = "You will be missed " + displayObject(*item) + ".";
	} else {
		dungeon->status = "You decided to keep the item around.";
		return;
	}
}

void Player::unequip(int index) {
	if (index < 0 || (uint32_t)index >= equipped.size()) {
		dungeon->status = "Cannot unequip - invalid equipment selection.";
		return;
	}

	if (inventory.size() >= (uint32_t)getMaxInventory()) {
		dungeon->status = "Your inventory is too full to unequip this item!";
		return;
	}

	auto item = equipped[index];
	equipped.erase(equipped.begin() + index);
	inventory.push_front(item);
	dungeon->status = "Your " + displayObject(*item) + " was put in your inventory.";
}

void Player::inspect(int index) {
	if (index < 0 || (uint32_t)index >= inventory.size()) {
		dungeon->status = "Cannot inspect - invalid inventory selection.";
		return;
	}
	int ch = 0;
	uint32_t offset = 0;
	auto item = inventory[index];

	vector<string> desc;
	desc.emplace_back("Damage:  " + item->getDamageString());
	desc.emplace_back("Weight:  " + to_string(item->getWeight()));
	desc.emplace_back("Defence: " + to_string(item->getDef()));
	desc.emplace_back("Speed:   " + to_string(item->getSpeed()));
	desc.emplace_back("Value:   " + to_string(item->getValue()));
	desc.insert(desc.end(), item->getDescription().begin(), item->getDescription().end());
	while(tickScroll(ch, offset, displayObject(*item) + ".", desc)) ch = getch();
}

void Player::drop(int index) {
	if (index < 0 || (uint32_t)index >= inventory.size()) {
		dungeon->status = "Cannot drop - invalid inventory selection.";
		return;
	}

	auto item = inventory[index];
	item->setPos(pos);
	inventory.erase(inventory.begin() + index);
	dungeon->getObjects().push_back(item);
	dungeon->status = displayObject(*item) + " was left on the ground.";
}

void Player::equip(int index) {
	if (index < 0 || (uint32_t)index >= inventory.size()) {
		dungeon->status = "Cannot equip - invalid inventory selection.";
		return;
	}

	auto newer = inventory[index];
	if (!newer->isEquipment()) {
		dungeon->status = "The item you tried to select is not equipment.";
		return;
	}

	int ringCount = 0;
	int ringCountMax = 2;
	for (auto it = equipped.begin(); it != equipped.end();) {
		shared_ptr<Object> older = *it;

		//Don't care about items of different types
		if ((older->getTypes() & newer->getTypes()) == 0) {
			it++;
			continue;
		}

		//Don't care about rings when the count is less than ring max
		if ((older->getTypes() & FObject::RING) > 0 && ringCount < ringCountMax - 1) {
			ringCount++;
			it++;
			continue;
		}

		//Item is of the same type, but player has no more space!
		if (inventory.size() >= (uint32_t)getMaxInventory()) {
			dungeon->status = "Your inventory is too full to equip this item.";
			return;
		}

		//Player has space, ask if they want to swap the item
		bool remove = choice({
			"You are about to equip the item:",
			"",
			displayObject(*newer),
			"",
			"This will unequip the item:",
			"",
			displayObject(*older),
			"",
			"The item will be returned to your inventory.",
			"Are you sure you want to do that?"
		});

		if (remove) {
			//Will increase inventory by one.
			inventory.push_back(older);
			it = equipped.erase(it);
		} else {
			//Let the user know the action failed.
			dungeon->status = "You decided not to wear that item.";
			return;
		}
	}

	//If we got through the previous loop, equip the item.
	inventory.erase(inventory.begin() + index);

	//insert the new item into the beginning
	//For the case of a ring, we remove the LAST ring,
	//so the user will always swap out their oldest.
	equipped.push_front(newer);
	dungeon->status = "You are now wearing " + displayObject(*newer);
}

bool Player::choice(const vector<string>& text) {
	werase(base);

	int row = 2;
	string question = "(Y / N)";

	//Draw question
	for (const auto& line : text) mvwprintw(base, row++, center(line), line.c_str());
	mvwprintw(base, row + 1, center(question), question.c_str());

	//Keep in loop until user presses Y/N
	while (true) {
		wrefresh(base);
		int in = getch();
		if (in == 'y' || in == 'Y') return true;
		if (in == 'n' || in == 'N') return false;
	}
}

void Player::tick() {
	action = AC_NONE;
	dungeon->status = "It's your turn! (TAB for help)";
	while (action == AC_NONE) tickInput();
	attack();
}

bool Player::tickScroll(int ch, uint &offset, const string& title, const vector<string>& constLines) {
	vector<string> lines = constLines;
	if (lines.empty()) lines.emplace_back("We've come up empty!");

	//Clear screen and figure out characters
	int max = LINES - 2;
	werase(base);
	switch (ch) {
		case KEY_UP:
			if (offset == 0) beep();
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
	action = AC_NONE;
	Movement res = MV_IDLE;
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
			action = AC_STALL;
			break;
		case '>':
			if (isOnEntity(STAIRS_DOWN)) {
				action = AC_DOWN;
			} else {
				dungeon->status = "You are not on a down staircase!";
			}
			break;
		case '<':
			if (isOnEntity(STAIRS_UP)) {
				action = AC_UP;
			} else {
				dungeon->status = "You are not on an up staircase!";
			}
			break;
		case 'Q':
			action = AC_QUIT;
			break;
		case 'q':
			dungeon->status = "If you really want to quit, try 'shift-q' instead.";
			break;
		case 'm':
			//keep ticking and getting characters until tick returns false
			for (const auto& m : dungeon->getMobs()) lines.push_back(displayMob(*m));
			while(tickScroll(ch, offset, "Monster list.", lines)) ch = getch();
			break;
		case '\t':
			while(tickScroll(ch, offset, "In game manual.", getHelp())) ch = getch();
			break;
		case 't':
		case 'e':
			list(equipped, "Unequip what?", 'a', &Player::unequip);
			break;
		case 'i':
		case 'w':
			list(inventory, "Equip what?", '0', &Player::equip);
			break;
		case 'x':
			list(inventory, "Expunge what?", '0', &Player::expunge);
			break;
		case 'd':
			list(inventory, "Drop what?", '0', &Player::drop);
			break;
		case 'I':
			list(inventory, "Inspect what?", '0', &Player::inspect);
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
	if (res == MV_FAIL) dungeon->status =  "I can't dig through that!";
	if (res == MV_SUCCESS) {
		action = AC_MOVE;
		dungeon->recalculate();
	}
}

Mob::Pickup Player::pickUpObject() {
	Pickup pickup = Mob::pickUpObject();
	switch(pickup) {
		case PICK_NONE:
			dungeon->status = "Nothing to pick up";
			break;
		case PICK_FULL:
			dungeon->status = "You are holding too many items";
			break;
		case PICK_WEIGHT:
			dungeon->status = "The item beneath you is too heavy to carry";
			break;
		case PICK_ADD:
			dungeon->status = displayObject(*inventory.front()) + " was added to your inventory.";
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
		"\ti, w",
		"\t\tShow your inventory and pick items to equip",
		"\te, t",
		"\t\tShow your equipped items ant pick items to unequip",
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
		"\tThis game uses Emoji (if enabled, see OPTIONS) and wide chars to get",
		"\tthe most out of your terminal emulator. This game will probably not work",
		"\tin any non UTF-8 conforming terminal (such as real terminals).",
		"\t",
		"\tHave fun killing monsters and defeating dungeons in ADVENTURE COMMANDER!",
		"",
		"OPTIONS",
	};

	vector<string> post = {
		"",
		"AUTHOR",
		"\tMade with <3 by Aaron Walter <ajwalter@iastate.edu>",
		"\t(Thanks for playing!)",
		"",
		"ADVENTURE COMMANDER(6)"
	};

	for(const auto& str : getOptions()) help.push_back("\t" + str);
	help.insert(help.end(), post.begin(), post.end());
	return help;
}

int Player::getCarryWeight() const {
	int weight = Mob::getCarryWeight();
	for (const auto& o : equipped) {
		weight += o->getWeight();
	}

	return weight;
}
