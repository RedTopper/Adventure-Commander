#include "path.hpp"
#include "mob.hpp"
#include "dungeon.hpp"

const int MAX_KNOWN_TURNS = 5;

Mob::Mob(Dungeon* dungeon, int turn) : Entity(dungeon, Entity::MOB) {
	known = 0;
	skills = rand() % 16;
	speed = (rand() % 16) + 5; //5-20
	order = turn + 1;
	hp = 1;
}

const Point Mob::getSpawn() {
	Point point;
	bool onMob = true;
	while(onMob) {
		point.x = rand() % dungeon->getDim().x;
		point.y = rand() % dungeon->getDim().y;
		const Tile& tile = dungeon->getTile(pos);

		//Spawn on floor
		if (!(tile.type == Tile::ROOM || tile.type == Tile::HALL)) continue;

		//Not on player
		if (pos == dungeon->getPlayer().pos) continue;

		//Not on mob
		onMob = false;
		for (const auto& m : dungeon->getMobs()) {
			if (point.x == m->pos.x) {
				onMob = true;
				break;
			};
		}
	}

	return point;
}

const wstring Mob::getSymbol() {
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
	out << getSymbol() << " at (" << pos.x << ", " << pos.y << ") is " << type << " and " << text;
	dungeon->status = out.str();
}

bool Mob::canSeePC() {
	Point current = Point(pos);
	Point end = dungeon->getPlayer().pos;
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
	Point next = nextPoint(dungeon->getPlayer().pos);
	Movement movement = move(next);

	wstring text;
	switch (movement) {
		case MOVE_BROKE_WALL:
			text = L"broke down the wall while beelining!";
			break;
		case MOVE_DAMAGE_WALL:
			text = L"damaged a wall while beelining!";
			break;
		case MOVE_SUCCESS:
			text = L"is beelining towards you!";
			break;
		default:
		case MOVE_FAILURE:
			text = L"tried to beeline, but failed!";
			break;
	}

	statusString(text, type);
}

void Mob::tickRandomly(const wchar_t* type) {
	int dir = rand() % (int)(sizeof(ADJACENT)/sizeof(ADJACENT[0]));
	Point next = Point(pos);
	next += ADJACENT[dir];
	Movement movement = move(next);

	wstring text;
	switch (movement) {
		case MOVE_BROKE_WALL:
			text = L"broke down the wall!";
			break;
		case MOVE_DAMAGE_WALL:
			text = L"chipped at the wall!";
			break;
		case MOVE_SUCCESS:
			text = L"moved to a new spot!";
			break;
		default:
		case MOVE_FAILURE:
			text = L"did nothing!";
			break;
	}

	statusString(text, type);
}

void Mob::tickPathFind(const wchar_t* type) {
	int adj = sizeof(ADJACENT)/sizeof(ADJACENT[0]);
	Point next;
	int lowest = INT32_MAX;
	for (int i = 0; i < adj; i++) {
		Point check;
		check = pos;
		check.x += ADJACENT[i].x;
		check.y += ADJACENT[i].y;

		int dist;
		if (skills & SKILL_TUNNELING) {
			dist = dungeon->getPathDig(check);
		} else {
			dist = dungeon->getPathFloor(check);
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
			case MOVE_BROKE_WALL:
				text = L"broke down the wall to get to you!";
				break;
			case MOVE_DAMAGE_WALL:
				text = L"damaged a wall to get to you!";
				break;
			case MOVE_SUCCESS:
				text = L"coming after you!";
				break;
			default:
			case MOVE_FAILURE:
				text = L"somehow failed to move?";
				break;
		}
		statusString(text, type);
	} else {
		tickRandomly(L"trapped");
	}
}

Mob::Movement Mob::move(Point& next) {
	Tile& tile = dungeon->getTile(next);
	if (tile.type == Tile::ROCK && skills & SKILL_TUNNELING) {
		if (tile.hardness <= HARDNESS_RATE) {
			tile.type = Tile::HALL;
			tile.hardness = 0;
			pos = next;
			pathCreate(dungeon);
			return MOVE_BROKE_WALL;
		} else {
			tile.hardness -= HARDNESS_RATE;
			pathCreate(dungeon);
			return MOVE_DAMAGE_WALL;
		}
	} else if (tile.type == Tile::HALL || tile.type == Tile::ROOM){
		pos = next;
		return MOVE_SUCCESS;
	}

	return MOVE_FAILURE;
}

void Mob::tick() {
	if (hp <= 0) return;
	dungeon->status = L"Nothing important happened.";

	if (skills & SKILL_ERRATIC && rand() % 2) {
		//Not the player, but erratic movement
		tickRandomly(L"confused");
	} else if (skills & SKILL_TELEPATHY) {
		//Not the player, not erratic, but has telepathy skill
		if (skills & SKILL_INTELLIGENCE) {
			tickPathFind(L"telepathic");
		} else {
			tickStraightLine(L"telepathic");
		}
	} else if (skills & SKILL_INTELLIGENCE) {
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
		const shared_ptr<Mob> other = (i < dungeon->getMobs().size()) ? dungeon->getMobs()[i] : make_shared<Player>(dungeon->getPlayer());

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

void mobGeneratePlayer(Dungeon* dungeon, Point point) {
	Mob* mob = malloc(sizeof(Mob));
	mob->pos = point;
	mob->known = 0;
	mob->skills = SKILL_PC;
	mob->speed = 10;
	mob->order = 0;
	mob->hp = 1;
	dungeon->player = mob;
}

void mobCreateQueue(Dungeon* dungeon) {
	Mob* player = dungeon->player;
	QueueNode* queue = queueCreateSub((QueueNodeData){.mob=player}, 1000 / player->speed, player->order);
	for (int i = 0; i < dungeon->numMobs; i++) {
		Mob* m = &dungeon->mobs[i];
		queuePushSub(&queue, (QueueNodeData){.mob=m}, 1000 / m->speed, m->order);
	}

	dungeon->turn = queue;
}
