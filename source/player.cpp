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
}

string Player::relative(const Mob& other) {
	stringstream str;
	str
		<< other.getSymbol()
		<< (other.isAlive() ? " is " : " was killed ")
		<< abs(pos.y - other.getPos().y)
		<< (other.getPos().y > pos.y ?  " south " : " north ")
		<< "and "
		<< abs(pos.x - other.getPos().x)
		<< (other.getPos().x < pos.x ? " west." : " east.");
	return str.str();
}

bool Player::tickMap(const int ch, uint& offset) {
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
			if (offset + max < dungeon->getMobs().size()) {
				offset++;
			} else {
				beep();
			}
			break;
		case 27: //ESC
		case 'q':
			return false;
		default:
			break;
	}

	//Show screen
	mvwaddstr(base, 0, 0, "Monster list. Press 'q' or 'ESC' to exit.");
	for (int line = 0; line < max; line++) {
		const shared_ptr<Mob> mob = line + offset < dungeon->getMobs().size() ? dungeon->getMobs()[line + offset] : nullptr;
		if (mob) {
			mvwaddstr(base, line + 1, 0, relative(*mob).c_str());
		} else {
			mvwaddstr(base, line + 1, 0, "~");
		}
	}

	//Print end marker if needed
	if (offset + max >= dungeon->getMobs().size()) {
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
			while(tickMap(ch, offset)) ch = getch();
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
	dungeon->status = "It's your turn!";
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
