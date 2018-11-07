#include "point.hpp"
#include "entity.hpp"
#include "dungeon.hpp"

Entity::Entity(Dungeon* dungeon, Type type, bool halls) {
	this->dungeon = dungeon;
	this->type = type;
	this->pos = getSpawn(halls);
	this->remembered = false;
}

const string Entity::getSymbol() const {
	const string ENT[] = {
		"?", //Mob has it's own definition.
		"\u25b2", //Up pointing triangle.
		"\u25bc", //Down pointing triangle.
	};

	const string ENT_BORING[] = {
		"?",
		"<",
		">",
	};

	if (dungeon->isFancy()) {
		return ENT[type];
	} else {
		return ENT_BORING[type];
	}
}

Point Entity::getSpawn(const bool halls) const {
	Point point;
	bool onEntity = true;
	while(onEntity) {
		point.x = rand() % dungeon->getDim().x;
		point.y = rand() % dungeon->getDim().y;
		const Tile& tile = dungeon->getTile(point);

		//Spawn on floor
		if (!(tile.type == Tile::ROOM || (halls && tile.type == Tile::HALL))) continue;

		//Not on player
		if (dungeon->getPlayer() && point == dungeon->getPlayer()->pos) continue;

		//Not on another entity
		onEntity = false;
		for (const auto& e : dungeon->getEntities()) {
			if (point == e.pos) {
				onEntity = true;
				break;
			}
		}
	}

	return point;
}
