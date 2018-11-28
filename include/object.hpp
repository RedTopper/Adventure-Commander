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
		const std::string& name,
		const std::string& symbol,
		const std::string& symbolAlt,
		const std::vector<std::string>& description
	);

private:
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
	bool isEquipment() const;

	Dice getDamage() const {
		return dam;
	}

	int getDef() const {
		return def;
	}

	int getSpeed() const {
		return speed;
	}

	int getValue() const {
		return value;
	}

	int getWeight() const {
		return weight;
	}

	int getTypes() const {
		return types;
	}

	std::string getDamageString() {
		std::stringstream str;
		str << dam;
		return str.str();
	}
};

#endif
