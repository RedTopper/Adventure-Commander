#ifndef ADVENTURE_COMMANDER_STREAM_ITEM_HPP
#define ADVENTURE_COMMANDER_STREAM_ITEM_HPP

#include <string>
#include <vector>

#include "dice.hpp"
#include "factory.hpp"
#include "object.hpp"

using namespace std;

class FObject : public Factory {
public:
	enum KeyWord {
		KWD_BAD   = 0,
		KWD_EMPTY = (1 << 0),
		BEGIN     = (1 << 1),
		OBJECT    = (1 << 2),
		NAME      = (1 << 3),
		DESC      = (1 << 4),
		TYPE      = (1 << 5),
		COLOR     = (1 << 6),
		HIT       = (1 << 7),
		DAM       = (1 << 8),
		DODGE     = (1 << 9),
		DEF       = (1 << 10),
		WEIGHT    = (1 << 11),
		SPEED     = (1 << 12),
		ATTR      = (1 << 13),
		VAL       = (1 << 14),
		ART       = (1 << 15),
		RRTY      = (1 << 16),
		END       = (1 << 17)
	};

	enum Type {
		TYPE_BAD   = 0,
		WEAPON     = (1 << 0),
		OFFHAND    = (1 << 1),
		RANGED     = (1 << 2),
		ARMOR      = (1 << 3),
		HELMET     = (1 << 4),
		CLOAK      = (1 << 5),
		GLOVES     = (1 << 6),
		BOOTS      = (1 << 7),
		RING       = (1 << 8),
		AMULET     = (1 << 9),
		LIGHT      = (1 << 10),
		SCROLL     = (1 << 11),
		BOOK       = (1 << 12),
		FLASK      = (1 << 13),
		GOLD       = (1 << 14),
		AMMUNITION = (1 << 15),
		FOOD       = (1 << 16),
		WAND       = (1 << 17),
		CONTAINER  = (1 << 18),
	};

private:
	int types = 0;
	int colors = 0;

	string name;
	vector<string> description;

	Dice hit;
	Dice dam;
	Dice dodge;
	Dice def;
	Dice weight;
	Dice speed;
	Dice attribute;
	Dice value;

public:
	bool isEquipment() const;
	static KeyWord toKeyWord(string word);
	static Type toType(string word);
	static string fromType(int type);
	Object get(Dungeon* dungeon, ...);

private:
	ostream& dump(ostream& out) const override;
	istream& read(istream& in) override;
	int getRequired() const override;
};

#endif
