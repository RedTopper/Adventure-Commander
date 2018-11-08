#ifndef ADVENTURE_COMMANDER_FACTORY_HPP
#define ADVENTURE_COMMANDER_FACTORY_HPP

#include "stream.hpp"

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

	bool isCreatable() const {
		return creatable;
	}

	//Sets if a monster can be recreated ever again.
	//Can only change this bit if the mob is unique.
	void notCreatable() {
		if (unique) creatable = false;
	}

	//Checks if the monster has been spawned in the dungeon.
	bool isSpawned() const {
		return spawned;
	}

	//Sets if this factory can produce another unique monster in this instance.
	//Can only change this bit if the mob is unique.
	void setSpawned(bool spawned) {
		if (unique) this->spawned = spawned;
	}
};

#endif
