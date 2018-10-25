#include "path.hpp"
#include "mob.hpp"
#include "dungeon.hpp"

const int MAX_KNOWN_TURNS = 5;

Mob::Mob(Dungeon* dungeon, int turn) : Entity(dungeon, Entity::MOB, true) {
	this->known = 0;
	this->skills = rand() % 16;
	this->speed = (rand() % 16) + 5; //5-20
	this->order = turn ;
	this->hp = 1;
	this->turn = 1000/speed;
}

const wstring Mob::getSymbol() const {
	const wchar_t* MOB_TYPES[] = {
		L"\x01F480", //Skull   (    )
		L"\x01F43A", //Angry   (I   )
		L"\x01F47B", //Ghost   ( T  )
		L"\x01F47F", //Demon   (IT  )
		L"\x01F417", //Boar    (  D )
		L"\x01F435", //Monkey  (I D )
		L"\x01F479", //Ogre    ( TD )
		L"\x01F432", //Dragon  (ITD )
		L"\x01F480", //Skull   (   E)
		L"\x01F419", //Squid   (I  E)
		L"\x01F40D", //Snake   ( D E)
		L"\x01F47A", //Goblin  (IT E)
		L"\x01F47E", //Monster (  DE)
		L"\x01F42F", //Tiger   (I DE)
		L"\x01F47D", //Alien   ( TDE)
		L"\x01F383", //Pumpkin (ITDE)
		L"\x01F464", //Person
		L"??"
	};

	const wchar_t* MOB_TYPES_BORING[] = {
		L"k", //Skull   (    )
		L"p", //Empty   (I   )
		L"g", //Ghost   ( T  )
		L"d", //Demon   (IT  )
		L"b", //Boar    (  D )
		L"m", //Monkey  (I D )
		L"O", //Ogre    ( TD )
		L"D", //Dragon  (ITD )
		L"k", //Skull   (   E)
		L"o", //Squid   (I  E)
		L"s", //Snake   ( D E)
		L"p", //Goblin  (IT E)
		L"M", //Monster (  DE)
		L"T", //Tiger   (I DE)
		L"A", //Alien   ( TDE)
		L"P", //Pumpkin (ITDE)
		L"@",
		L"?"
	};

	if (dungeon->isFancy()) {
		return MOB_TYPES[skills];
	} else {
		return MOB_TYPES_BORING[skills];
	}
}

void Mob::statusString(const wstring& text, const wstring& type) {
	wstringstream out;
	out
		<< getSymbol()
		<< " on turn "
		<< turn
		<< " at ("
		<< pos.x
		<< ", "
		<< pos.y
		<< ") is "
		<< type
		<< " and "
		<< text;

	dungeon->status = out.str();
}

bool Mob::canSeePC() {
	Point current = Point(pos);
	Point end = dungeon->getPlayer()->pos;
	Point sign;
	Point diff;
	diff.x = abs(end.x - current.x),
	diff.y = abs(end.y - current.y),
	sign.x = current.x < end.x ? 1 : -1;
	sign.y = current.y < end.y ? 1 : -1;

	int err = (diff.x > diff.y ? diff.x : -diff.y)/2, e2;

	while(!(current.x == end.x && current.y == end.y)) {
		const Tile& tile = dungeon->getTile(current);
		if (!(tile.type == Tile::HALL || tile.type == Tile::ROOM)) return false;

		e2 = err;
		if (e2 > -diff.x) {
			err -= diff.y;
			current.x += sign.x;
		}

		if (e2 <  diff.y) {
			err += diff.x;
			current.y += sign.y;
		}
	}

	return true;
}

Point Mob::nextPoint(Point end) {
	if (pos == end) return pos;
	Point current = pos;
	Point sign;
	Point diff;
	diff.x = abs(end.x - current.x),
	diff.y = abs(end.y - current.y),
	sign.x = current.x < end.x ? 1 : -1;
	sign.y = current.y < end.y ? 1 : -1;

	int err = (diff.x > diff.y ? diff.x : -diff.y)/2;
	if (err > -diff.x) {
		current.x += sign.x;
	}

	if (err <  diff.y) {
		current.y += sign.y;
	}

	return current;
}

void Mob::tickStraightLine(const wchar_t* type) {
	Point next = nextPoint(dungeon->getPlayer()->pos);
	Movement movement = move(next);

	wstring text;
	switch (movement) {
		case BROKE_WALL:
			text = L"broke down the wall while beelining!";
			break;
		case DAMAGE_WALL:
			text = L"damaged a wall while beelining!";
			break;
		case SUCCESS:
			text = L"is beelining towards you!";
			break;
		default:
		case FAILURE:
			text = L"tried to beeline, but failed!";
			break;
	}

	statusString(text, type);
}

void Mob::tickRandomly(const wchar_t* type) {
	int dir = rand() % (int)(sizeof(Path::ADJACENT)/sizeof(Path::ADJACENT[0]));
	Point next = Point(pos);
	next += Path::ADJACENT[dir];
	Movement movement = move(next);

	wstring text;
	switch (movement) {
		case BROKE_WALL:
			text = L"broke down the wall!";
			break;
		case DAMAGE_WALL:
			text = L"chipped at the wall!";
			break;
		case SUCCESS:
			text = L"moved to a new spot!";
			break;
		default:
		case FAILURE:
			text = L"did nothing!";
			break;
	}

	statusString(text, type);
}

void Mob::tickPathFind(const wchar_t* type) {
	int adj = sizeof(Path::ADJACENT)/sizeof(Path::ADJACENT[0]);
	Point next;
	int lowest = INT32_MAX;
	for (int i = 0; i < adj; i++) {
		Point check;
		check = pos;
		check += Path::ADJACENT[i];

		int dist;
		if (skills & TUNNELING) {
			dist = dungeon->getPathDig(check);
		} else {
			dist = dungeon->getPathMap(check);
		}

		if (dist < lowest) {
			lowest = dist;
			next = check;
		}
	}

	if (lowest < INT32_MAX) {
		Movement movement;
		movement = move(next);
		wstring text;
		switch (movement) {
			case BROKE_WALL:
				text = L"broke down the wall to get to you!";
				break;
			case DAMAGE_WALL:
				text = L"damaged a wall to get to you!";
				break;
			case SUCCESS:
				text = L"coming after you!";
				break;
			default:
			case FAILURE:
				text = L"somehow failed to move?";
				break;
		}
		statusString(text, type);
	} else {
		tickRandomly(L"trapped");
	}
}

Mob::Movement Mob::move(const Point& next) {
	Tile& tile = dungeon->getTile(next);
	if (tile.type == Tile::ROCK && skills & TUNNELING) {
		if (tile.hardness <= Path::HARDNESS_RATE) {
			tile.type = Tile::HALL;
			tile.hardness = 0;
			pos = next;
			dungeon->recalculate();
			return BROKE_WALL;
		} else {
			tile.hardness -= Path::HARDNESS_RATE;
			dungeon->recalculate();
			return DAMAGE_WALL;
		}
	} else if (tile.type == Tile::HALL || tile.type == Tile::ROOM){
		pos = next;
		return SUCCESS;
	}

	return FAILURE;
}

void Mob::tick() {
	if (hp <= 0) return;
	dungeon->status = L"Nothing important happened.";

	if (skills & ERRATIC && rand() % 2) {
		//Not the player, but erratic movement
		tickRandomly(L"confused");
	} else if (skills & TELEPATHY) {
		//Not the player, not erratic, but has telepathy skill
		if (skills & INTELLIGENCE) {
			tickPathFind(L"telepathic");
		} else {
			tickStraightLine(L"telepathic");
		}
	} else if (skills & INTELLIGENCE) {
		//Not the player, not erratic, not telepathic, but has intelligence
		if (canSeePC()) known = MAX_KNOWN_TURNS;
		if (known > 0) {
			tickPathFind(L"smart");
			known--;
		} else {
			tickRandomly(L"searching");
		}
	} else {
		//Not the player, not erratic, not telepathic, not intelligent, but... idk, exists?
		if (canSeePC()) {
			tickStraightLine(L"a bumbling idiot");
		} else {
			tickRandomly(L"a bumbling idiot");
		}
	}

	//Attack phase
	for (int i = 0; i < dungeon->getMobs().size() + 1; i++) {
		//Make sure to include the player in the attack phase
		const shared_ptr<Mob> other = (i < dungeon->getMobs().size()) ? dungeon->getMobs()[i] : dungeon->getPlayer();

		//Collision detection. Monsters are 2 wide when emoji is enabled on most systems
		if (!(order != other->order
			&& other->hp != 0
			&& (pos.x == other->pos.x || (dungeon->isFancy() && (pos.x + 1 == other->pos.x || pos.x - 1 == other->pos.x)))
			&& pos.y == other->pos.y)) continue;

		other->hp--;
		wstring text = (other->hp == 0) ? L"It killed it brutally!" : L"Looks like it hurt!";
		wstringstream status;
		status << getSymbol() << " at (" << pos.x << ", " << pos.y << ") attacked " << other->getSymbol() << " at (" << other->pos.x << ", " << other->pos.y << ")! " << text;
		dungeon->status = status.str();
	}
}

const bool Mob::isOn(Entity::Type type) const {
	for (auto& e : dungeon->getEntities()) {
		if (pos == e.getPos() && e.getType() == type) return true;
	}

	return false;
}

const bool Mob::isBefore(const Mob &other) const {
	if (turn < other.getTurn()) {
		return true;
	} else if (turn == other.getTurn()) {
		return order < other.getOrder();
	} else {
		return false;
	};
}

