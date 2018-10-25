#include "point.hpp"
#include "entity.hpp"
#include "dungeon.hpp"

Entity::Entity(Dungeon* dungeon, Type type, bool halls) {
	this->dungeon = dungeon;
	this->type = type;
	this->pos = getSpawn(halls);
}

const wstring Entity::getSymbol() const {
	const wstring ENT[] = {
		L"?", //Mob has it's own definition.
		L"\x25b2", //Up pointing triangle.
		L"\x25bc", //Down pointing triangle.
	};

	const wstring ENT_BORING[] = {
		L"?", //Mob has it's own definition.
		L"\x25b2", //Up pointing triangle.
		L"\x25bc", //Down pointing triangle.
	};

	if (dungeon->isFancy()) {
		return ENT[type];
	} else {
		return ENT_BORING[type];
	}
}

const Point Entity::getSpawn(const bool halls) const {
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
