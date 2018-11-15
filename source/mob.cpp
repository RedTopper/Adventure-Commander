
#include <mob.hpp>

#include "path.hpp"
#include "mob.hpp"
#include "dungeon.hpp"

const int MAX_KNOWN_TURNS = 5;

Mob::Mob(
	Dungeon* dungeon,
	FMob* factory,
	Color color,
	int order,
	int skills,
	int speed,
	int hp,
	const Dice& dam,
	const string& name,
	const string& symbol,
	const string& symbolAlt,
	const vector<string>& description
) : Entity(dungeon, Entity::OTHER, color, true) {
	this->known = 0;
	this->order = order;
	this->skills = skills;
	this->speed = speed;
	this->hp = hp;
	this->dam = dam;
	this->name = name;
	this->symbol = symbol;
	this->symbolAlt = symbolAlt;
	this->description = description;
	this->turn = 1000/speed;
	this->factory = factory;
}

void Mob::statusString(const string& text, const string& type) {
	stringstream out;
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

void Mob::tickStraightLine(const string& type) {
	Point next = nextPoint(dungeon->getPlayer()->pos);
	Movement movement = move(next);

	string text;
	switch (movement) {
		case MV_DESTROY:
			text = "broke down the wall while beelining!";
			break;
		case MV_DAMAGE:
			text = "damaged a wall while beelining!";
			break;
		case MV_SUCCESS:
			text = "is beelining towards you!";
			break;
		default:
		case MV_FAIL:
			text = "tried to beeline, but failed!";
			break;
	}

	statusString(text, type);
}

void Mob::tickRandomly(const string& type) {
	int dir = rand() % (int)(sizeof(Path::ADJACENT)/sizeof(Path::ADJACENT[0]));
	Point next = Point(pos);
	next += Path::ADJACENT[dir];
	Movement movement = move(next);

	string text;
	switch (movement) {
		case MV_DESTROY:
			text = "broke down the wall!";
			break;
		case MV_DAMAGE:
			text = "chipped at the wall!";
			break;
		case MV_SUCCESS:
			text = "moved to a new spot!";
			break;
		default:
		case MV_FAIL:
			text = "did nothing!";
			break;
	}

	statusString(text, type);
}

void Mob::tickPathFind(const string& type) {
	int adj = sizeof(Path::ADJACENT)/sizeof(Path::ADJACENT[0]);
	Point next;
	int lowest = INT32_MAX;
	for (int i = 0; i < adj; i++) {
		Point check;
		check = pos;
		check += Path::ADJACENT[i];

		int dist;
		if (skills & SK_TUNNELING) {
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
		string text;
		switch (movement) {
			case MV_DESTROY:
				text = "broke down the wall to get to you!";
				break;
			case MV_DAMAGE:
				text = "damaged a wall to get to you!";
				break;
			case MV_SUCCESS:
				text = "coming after you!";
				break;
			default:
			case MV_FAIL:
				text = "somehow failed to move?";
				break;
		}
		statusString(text, type);
	} else {
		tickRandomly("trapped");
	}
}

Mob::Movement Mob::move(const Point& next) {
	Tile& tile = dungeon->getTile(next);
	if (tile.type == Tile::ROCK && skills & SK_TUNNELING) {
		if (tile.hardness <= Path::HARDNESS_RATE) {
			tile.type = Tile::HALL;
			tile.hardness = 0;
			pos = next;
			dungeon->recalculate();
			return MV_DESTROY;
		} else {
			tile.hardness -= Path::HARDNESS_RATE;
			dungeon->recalculate();
			return MV_DAMAGE;
		}
	} else if (tile.type == Tile::HALL || tile.type == Tile::ROOM){
		pos = next;
		return MV_SUCCESS;
	}

	return MV_FAIL;
}

void Mob::tick() {
	if (hp <= 0) return;
	dungeon->status = "Nothing important happened.";

	if (skills & SK_ERRATIC && rand() % 2) {
		//Not the player, but erratic movement
		tickRandomly("confused");
	} else if (skills & SK_TELEPATHY) {
		//Not the player, not erratic, but has telepathy skill
		if (skills & SK_INTELLIGENCE) {
			tickPathFind("telepathic");
		} else {
			tickStraightLine("telepathic");
		}
	} else if (skills & SK_INTELLIGENCE) {
		//Not the player, not erratic, not telepathic, but has intelligence
		if (canSeePC()) known = MAX_KNOWN_TURNS;
		if (known > 0) {
			tickPathFind("smart");
			known--;
		} else {
			tickRandomly("searching");
		}
	} else {
		//Not the player, not erratic, not telepathic, not intelligent, but... idk, exists?
		if (canSeePC()) {
			tickStraightLine("a bumbling idiot");
		} else {
			tickRandomly("a bumbling idiot");
		}
	}

	attack();
}

void Mob::attack() {
	//Attack phase
	for (uint i = 0; i < dungeon->getMobs().size() + 1; i++) {
		//Make sure to include the player in the attack phase
		const shared_ptr<Mob> other = (i < dungeon->getMobs().size()) ? dungeon->getMobs()[i] : dungeon->getPlayer();

		//Collision detection.
		if (order == other->order || other->hp == 0 || pos != other->pos) continue;

		other->hp--;
		string text = "Looks like it hurt!";
		if (hp == 0) {
			text = "It killed it brutally!";
			if (factory) factory->notCreatable();
		}

		stringstream status;
		status
			<< getSymbol()
			<< " at ("
			<< pos.x
			<< ", "
			<< pos.y
			<< ") attacked "
			<< other->getSymbol()
			<< " at ("
			<< other->pos.x
			<< ", "
			<< other->pos.y
			<< ")! "
			<< text;

		dungeon->status = status.str();
	}
}

bool Mob::isOnEntity(Entity::Type type) const {
	for (auto& e : dungeon->getEntities()) {
		if (pos == e.getPos() && e.getEntityType() == type) return true;
	}

	return false;
}

Mob::Pickup Mob::pickUpObject() {
	//First check if we can pick up item
	if (inventory.size() == (uint32_t)(getMaxInventory())) return PICK_FULL;

	auto& objects = dungeon->getObjects();
	for (auto it = objects.begin(); it != objects.end();) {
		auto& o = *it;
		if (pos != o->getPos()) {
			//Not on this item
			it++;
		} else if (o->getWeight() + getCarryWeight() > getMaxCarryWeight()) {
			//Item we will pick up exceeds max capacity
			return PICK_WEIGHT;
		} else {
			//Pick up item from floor
			inventory.push_front(o);
			it = objects.erase(it);
			return PICK_ADD;
		}
	}

	return PICK_NONE;
}

bool Mob::isBefore(const Mob& other) const {
	if (turn < other.getTurn()) {
		return true;
	} else if (turn == other.getTurn()) {
		return order < other.getOrder();
	} else {
		return false;
	};
}

int Mob::getCarryWeight() const {
	int weight = 0;
	for (const auto& o : inventory) {
		weight += o->getWeight();
	}

	return weight;
}
