#include <algorithm>

#include "path.hpp"
#include "mob.hpp"
#include "dungeon.hpp"

using namespace std;

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

void Mob::statusString(const string& type, Movement move) {
	//Status text set in attack
	if (move == MV_ATTACK) return;

	string text;
	switch(move) {
		case MV_FAIL:
			text = "failed to move!";
			break;
		case MV_SUCCESS:
			text = "moved!";
			break;
		case MV_DAMAGE:
			text = "damaged a wall!";
			break;
		case MV_DESTROY:
			text = "destroyed a wall!";
			break;
		default:
			text = "looked alive!";
	}

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

int Mob::damage(int dam) {
	hp -= dam;
	if (hp < 0) hp = 0;
	factory->notCreatable();
	return dam;
}

void Mob::attack(const Point&) {
	int damage = dungeon->getPlayer()->damage(getDamage());
	dungeon->status = "(" + getSymbol() + ") " + getName() + " damaged you for " + to_string(damage) + "! [ANY KEY]";
	dungeon->getPlayer()->flip();
}

Mob::Movement Mob::move(const Point& next) {
	//If the place we move to is the player, attack!
	if (next == dungeon->getPlayer()->getPos()) {
		attack(next);
		return MV_ATTACK;
	}

	//Check if the mob can tunnel
	Tile& tile = dungeon->getTile(next);
	if (tile.type == Tile::ROCK && skills & SK_TUNNELING) {
		//Can tunnel
		if (tile.hardness <= Path::HARDNESS_RATE) {
			//Broke down a wall
			tile.type = Tile::HALL;
			tile.hardness = 0;
			pos = next;
			dungeon->recalculate();
			return MV_DESTROY;
		} else {
			//Simply damaged a wall
			tile.hardness -= Path::HARDNESS_RATE;
			dungeon->recalculate();
			return MV_DAMAGE;
		}
	}

	//So it can't tunnel
	//Are we moving into an open space?
	if (tile.type == Tile::HALL || tile.type == Tile::ROOM){
		//Check if there is a mob where we are going to move
		const auto& mobNext = dungeon->getMob(next);
		if (mobNext != nullptr) {
			//So there is, we have to push it out of the way
			bool pushed = false;

			//Randomly flip the adjacent array for randomness
			vector<Point> adjacent = Path::getAdjacent();
			if (rand() % 2) reverse(adjacent.begin(), adjacent.end());
			for(const auto& dir : adjacent) {
				Point adj = Point() + dir + next;

				//Skip adjacent tiles that are not free space
				const auto mobNextAdjTile = dungeon->getTile(adj).type;
				if (!(mobNextAdjTile == Tile::ROOM || mobNextAdjTile == Tile::HALL)) continue;

				//Skip the player
				if (adj == dungeon->getPlayer()->getPos()) continue;

				//Skip adjacent tiles that have mobs
				auto mobNextAdj = dungeon->getMob(adj);
				if (mobNextAdj != nullptr) continue;

				//We found an open space
				mobNext->setPos(adj);
				pushed = true;
				break;
			}

			//We did not push the next monster, so we'll swap it
			if (!pushed) {
				mobNext->setPos(pos);
			}
		}

		//Update the mobs current position
		pos = next;
		return MV_SUCCESS;
	}

	//What?
	return MV_FAIL;
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
	statusString(type, move(next));
}

void Mob::tickRandomly(const string& type) {
	int dir = rand() % (int)Path::getAdjacent().size();
	Point next = Point(pos);
	next += Path::getAdjacent()[dir];
	statusString(type, move(next));
}

void Mob::tickPathFind(const string& type) { ;
	Point next;
	int lowest = INT32_MAX;
	for (int i = 0; i < (int)Path::getAdjacent().size(); i++) {
		Point check;
		check = pos;
		check += Path::getAdjacent()[i];

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
		statusString(type, move(next));
	} else {
		tickRandomly("trapped");
	}
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
