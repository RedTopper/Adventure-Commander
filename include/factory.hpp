#ifndef ADVENTURE_COMMANDER_FACTORY_HPP
#define ADVENTURE_COMMANDER_FACTORY_HPP

#include <memory>

#include "stream.hpp"

class Entity;
class Dungeon;
class Factory : public Stream {
protected:
	int rarity = 0;
	bool creatable = true;
	bool spawned = false;
	bool unique = false;

public:
	int getRarity() {
		return rarity;
	}

	//Checks if a entity is re-creatable (ie: hasn't been killed/destroyed)
	bool isCreatable() const {
		return creatable;
	}

	//Checks if the entity has been spawned in the dungeon.
	bool isSpawned() const {
		return spawned;
	}

	//Sets if a entity can be recreated ever again.
	//Can only change this bit if the entity is unique.
	void notCreatable() {
		if (unique) creatable = false;
	}

	//Sets if this factory can produce another unique entity in this instance.
	//Can only change this bit if the entity is unique.
	void setSpawned(bool spawned) {
		if (unique) this->spawned = spawned;
	}

	virtual const std::shared_ptr<Entity> make(Dungeon *dungeon, int turn) = 0;
};

#endif
