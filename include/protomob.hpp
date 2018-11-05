#ifndef ADVENTURE_COMMANDER_MOB_HPP
#define ADVENTURE_COMMANDER_MOB_HPP

#include <string>
#include <vector>
#include "dice.hpp"
#include "stream.hpp"
#include "mob.hpp"

using namespace std;

class ProtoMob : public Stream {
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
	static const int REQUIRED = (
		  NAME
		| DESC
		| COLOR
		| SPEED
		| ABIL
		| HP
		| DAM
		| SYMB
		| RRTY
		| END
	);

	int keywords = 0;
	int rarity = 0;
	int colors = 0;
	int abilities = 0;

	string name;
	string symbol;
	vector<string> description;

	Dice hp;
	Dice dam;
	Dice speed;

public:
	static KeyWord toKeyWord(string word);
	static Mob::Color toColor(string color);
	static Mob::Skills toSkill(string skill);

	bool isValid() const {
		//keywords contains all REQUIRED bits, other bits don't care
		return (keywords & REQUIRED) == REQUIRED;
	}

private:
	ostream& dump(ostream& out) const override;
	istream& read(istream& in) override;
};

#endif
