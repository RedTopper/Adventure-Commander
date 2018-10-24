
#include <entity.hpp>

#include "point.hpp"
#include "entity.hpp"
#include "dungeon.hpp"

Entity::Entity(Dungeon* dungeon, Type type) {
	this->dungeon = dungeon;
	this->type = type;
	this->pos = getSpawn();
}

const wstring Entity::getSymbol() {
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

const Point Entity::getSpawn() {
	Point point;
	bool onEntity = true;
	while(onEntity) {
		point.x = rand() % dungeon->getDim().x;
		point.y = rand() % dungeon->getDim().y;
		const Tile& tile = dungeon->getTile(pos);

		//Spawn on floor
		if (tile.type != Tile::ROOM) continue;

		//Not on player
		if (point == dungeon->getPlayer().pos) continue;

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
