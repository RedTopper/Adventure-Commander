#include "point.hpp"
#include "entity.hpp"
#include "dungeon.hpp"
#include "twist.hpp"
#include "player.hpp"

using namespace std;

Entity::Entity(Dungeon* dungeon, Type type, Color color, bool halls) {
	const string ENT[] = {
		u8"?", //Mob has it's own definition.
		u8"\U0001F53A", //Up pointing triangle.
		u8"\U0001F53B", //Down pointing triangle.
	};

	const string ENT_BORING[] = {
		"?",
		"<",
		">",
	};

	this->dungeon = dungeon;
	this->type = type;
	this->pos = getSpawn(halls);
	this->remembered = false;
	this->color = color;
	this->symbol = ENT[type];
	this->symbolAlt = ENT_BORING[type];
	this->name = symbol;
}

Point Entity::getSpawn(const bool halls) const {
	Point point;
	bool onEntity = true;
	while(onEntity) {
		point = Twist::rand(Point(1), dungeon->getDim() - 2);
		const Tile& tile = dungeon->getTile(point);

		//Spawn on floor
		if (!(tile.type == Tile::ROOM || (halls && tile.type == Tile::HALL))) continue;

		//Not on player
		if (dungeon->getPlayer() && point == dungeon->getPlayer()->pos) continue;

		//Not on another entity
		onEntity = false;
		for (const auto& e : dungeon->getEntities()) {
			if (point == e->pos) {
				onEntity = true;
				break;
			}
		}
	}

	return point;
}

const string Entity::getSymbol() const {
	if (dungeon->isFancy()) {
		return this->symbol;
	} else {
		return this->symbolAlt;
	}
}

void Entity::setRemembered(bool r) {
	if (dungeon->getDisplay() == Dungeon::FOGGY) remembered = r;
}
