#ifndef ADVENTURE_COMMANDER_STREAM_ITEM_HPP
#define ADVENTURE_COMMANDER_STREAM_ITEM_HPP

#include <string>
#include <vector>

#include "dice.hpp"
#include "factory.hpp"

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
	unsigned int types = 0;
	unsigned int colors = 0;

	std::string name;
	std::vector<std::string> description;

	Dice hit;
	Dice dam;
	Dice dodge;
	Dice def;
	Dice weight;
	Dice speed;
	Dice attribute;
	Dice value;

public:
	static KeyWord toKeyWord(std::string word);
	static Type toType(std::string word);
	static std::string fromType(int type);
	const std::shared_ptr<Entity> make(Dungeon *dungeon, int) override;

private:
	std::ostream& dump(std::ostream& out) const override;
	std::istream& read(std::istream& in) override;
	unsigned int getRequired() const override;
};

#endif
