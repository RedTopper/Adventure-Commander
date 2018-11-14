#ifndef ADVENTURE_COMMANDER_OBJECT_HPP
#define ADVENTURE_COMMANDER_OBJECT_HPP

#include <vector>

#include "entity.hpp"
#include "dice.hpp"

class FObject;
class Object : public Entity {
public:
	Object(
		Dungeon *dungeon,
		FObject* factory,
		Color color,
		int types,
		int hit,
		int dodge,
		int def,
		int weight,
		int speed,
		int attribute,
		int value,
		const Dice& dam,
		const string& name,
		const string& symbol,
		const string& symbolAlt,
		const vector<string>& description
	);

private:
	FObject* factory;
	Dice dam;
	int types;
	int hit;
	int dodge;
	int def;
	int weight;
	int speed;
	int attribute;
	int value;

public:
	int getWeight() const {
		return weight;
	}

	int getTypes() const {
		return types;
	}
};

#endif
