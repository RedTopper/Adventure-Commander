#ifndef ADVENTURE_COMMANDER_STREAM_MONSTER_HPP
#define ADVENTURE_COMMANDER_STREAM_MONSTER_HPP

#include <string>
#include <vector>

#include "dice.hpp"
#include "factory.hpp"
#include "mob.hpp"

class FMob : public Factory {
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
	int colors = 0;
	int abilities = 0;

	std::string name;
	std::string symbol;
	std::string symbolAlt;
	std::vector<std::string> description;

	Dice hp;
	Dice dam;
	Dice speed;

public:
	static KeyWord toKeyWord(std::string word);
	static Entity::Color toColor(std::string color);
	static Mob::Skills toSkill(std::string skill);
	Mob get(Dungeon* dungeon, int turn);

private:
	std::ostream& dump(std::ostream& out) const override;
	std::istream& read(std::istream& in) override;
	int getRequired() const override;
};

#endif
