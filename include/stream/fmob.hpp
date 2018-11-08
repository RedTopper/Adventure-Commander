#ifndef ADVENTURE_COMMANDER_STREAM_MONSTER_HPP
#define ADVENTURE_COMMANDER_STREAM_MONSTER_HPP

#include <string>
#include <vector>

#include "dice.hpp"
#include "stream.hpp"
#include "mob.hpp"
#include "entity.hpp"

using namespace std;

class FMob : public Stream {
public:
	enum KeyWord {
		KWD_BAD   = 0,
		KWD_EMPTY = (1 << 0),
		BEGIN     = (1 << 1),
		MONSTER   = (1 << 2),
		NAME      = (1 << 3),
		DESC      = (1 << 4),
		COLOR     = (1 << 5),
		SPEED     = (1 << 6),
		ABIL      = (1 << 7),
		HP        = (1 << 8),
		DAM       = (1 << 9),
		SYMB      = (1 << 10),
		RRTY      = (1 << 11),
		END       = (1 << 12),
	};

private:
	bool creatable = true;
	bool spawned = false;

	int rarity = 0;
	int colors = 0;
	int abilities = 0;

	string name;
	string symbol;
	string symbolAlt;
	vector<string> description;

	Dice hp;
	Dice dam;
	Dice speed;

public:
	static KeyWord toKeyWord(string word);
	static Entity::Color toColor(string color);
	static Mob::Skills toSkill(string skill);
	Mob getMob(Dungeon* dungeon, int turn);
	int getRarity() {
		return rarity;
	}

	bool isCreatable() const {
		return creatable;
	}

	//Sets if a monster can be recreated ever again.
	//Can only change this bit if the mob is unique.
	void notCreatable() {
		if (abilities & Mob::UNIQUE) creatable = false;
	}

	//Checks if the monster has been spawned in the dungeon.
	bool isSpawned() const {
		return spawned;
	}

	//Sets if this factory can produce another unique monster in this instance.
	//Can only change this bit if the mob is unique.
	void setSpawned(bool spawned) {
		if (abilities & Mob::UNIQUE) this->spawned = spawned;
	}

private:
	ostream& dump(ostream& out) const override;
	istream& read(istream& in) override;
	int getRequired() const override;
};

#endif
