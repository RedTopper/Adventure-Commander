#define _XOPEN_SOURCE_EXTENDED

#include "dungeon.hpp"

Player::Player(Dungeon *dungeon, WINDOW *window) : Mob(dungeon, 0) {
	this->base = window;
	this->action = NONE;
	this->skills = PC;
	this->known = 0;
	this->speed = 10;
	this->hp = 1;
	this->turn = 1000/speed;
}

wstring Player::relative(const Mob& other) {
	wstringstream str;
	str
		<< other.getSymbol()
		<< (other.isAlive() ? L" is " : L" was killed ")
		<< abs(pos.y - other.getPos().y)
		<< (other.getPos().y > pos.y ?  L" south " : L" north ")
		<< "and "
		<< abs(pos.x - other.getPos().x)
		<< (other.getPos().x < pos.x ? L" west." : L" east.");
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
	mvwaddwstr(base, 0, 0, L"Monster list. Press 'q' or 'ESC' to exit.");
	for (int line = 0; line < max; line++) {
		const shared_ptr<Mob> mob = line + offset < dungeon->getMobs().size() ? dungeon->getMobs()[line + offset] : nullptr;
		if (mob) {
			mvwaddwstr(base, line + 1, 0, relative(*mob).c_str());
		} else {
			mvwaddwstr(base, line + 1, 0, L"~");
		}
	}

	//Print end marker if needed
	if (offset + max >= dungeon->getMobs().size()) {
		wattron(base, WA_STANDOUT);
		mvwaddwstr(base, LINES - 1, 0, L"(END)");
		wattroff(base, WA_STANDOUT);
	} else {
		mvwaddwstr(base, LINES - 1, 0, L":");
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
			dungeon->status = L"Woosh!";
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
	mvwaddwstr(base, dest.y + 1, dest.x, L"\x00D7");

	return true;
}

void Player::tick() {
	dungeon->updateFoggy();
	dungeon->print(base);
	action = NONE;
	Movement res = IDLE;
	int ch = getch();
	uint offset = 0;
	Point dest = pos;
	switch (ch) {
		case '7':
		case 'y':
			res = move(Point(pos) += Point(-1,-1));
			break;
		case '8':
		case 'k':
			res = move(Point(pos) += Point(0,-1));
			break;
		case '9':
		case 'u':
			res = move(Point(pos) += Point(1,-1));
			break;
		case '6':
		case 'l':
			res = move(Point(pos) += Point(1,0));
			break;
		case '3':
		case 'n':
			res = move(Point(pos) += Point(1,1));
			break;
		case '2':
		case 'j':
			res = move(Point(pos) += Point(0,1));
			break;
		case '1':
		case 'b':
			res = move(Point(pos) += Point(-1,1));
			break;
		case '4':
		case 'h':
			res = move(Point(pos) += Point(-1,0));
			break;
		case '5':
		case ' ':
			action = STALL;
			break;
		case '>':
			if (isOn(STAIRS_DOWN)) {
				action = DOWN;
			} else {
				dungeon->status = L"You are not on a down staircase!";
			}
			break;
		case '<':
			if (isOn(STAIRS_UP)) {
				action = UP;
			} else {
				dungeon->status = L"You are not on an up staircase!";
			}
			break;
		case 'Q':
			action = QUIT;
			break;
		case 'q':
			dungeon->status = L"If you really want to quit, try 'shift-q' instead.";
			break;
		case 'm':
			//keep ticking and getting characters until tick returns false
			while(tickMap(ch, offset)) ch = getch();
			break;
		case 'g':
			ch = 0;
			dungeon->status = L"You cheater! You entered teleport mode!";
			while(tickTarget(ch, dest)) ch = getch();
			break;
		case 'f':
			dungeon->status = L"You cheater! You toggled the fog!";
			dungeon->setFoggy(!dungeon->isFoggy());
			break;
		default:
			dungeon->status = to_wstring(ch) + L" is invalid!";
	}

	//Move the player character
	if (res == FAILURE) dungeon->status =  L"I can't dig through that!";
	if (res == SUCCESS) {
		action = MOVE;
		dungeon->recalculate();
	}

	attack();
}
