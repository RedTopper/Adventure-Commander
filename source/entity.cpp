
#include <entity.hpp>

#include "point.hpp"
#include "entity.hpp"
#include "dungeon.hpp"

const wstring Entity::getSymbol() {
	const wstring symbols[] = {
			L"?", //Mob has it's own definition.
			L"\x25b2", //Up pointing triangle.
			L"\x25bc", //Down pointing triangle.
	};

	return symbols[type];
}

Entity::Entity(Dungeon* dungeon, Type type) {
	this->dungeon = dungeon;
	this->type = type;

	bool onEntity = true;
	while(onEntity) {
		pos.x = rand() % dungeon->getDim().x;
		pos.y = rand() % dungeon->getDim().y;
		const Tile& tile = dungeon->getTiles()[pos.y][pos.x];

		//Spawn on floor
		if (tile.type != Tile::ROOM) continue;

		//Not on player
		if (pos == dungeon->getPlayer().pos) continue;

		//Not on another entity
		onEntity = false;
		for (const auto& e : dungeon->getEntities()) {
			if (pos == e.pos) {
				onEntity = true;
				break;
			}
		}
	}
}
