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
		BAD_KWD,
		BEGIN,
		MONSTER,
		NAME,
		DESC,
		COLOR,
		SPEED,
		ABIL,
		HP,
		DAM,
		SYMB,
		RRTY,
		END,
	};

private:
	bool valid = false;
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
		return valid;
	}

private:
	void invalidate(istream &in);
	ostream& dump(ostream& out) const override;
	istream& read(istream& in) override;
};

#endif
