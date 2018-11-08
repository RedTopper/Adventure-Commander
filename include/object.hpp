#ifndef ADVENTURE_COMMANDER_OBJECT_HPP
#define ADVENTURE_COMMANDER_OBJECT_HPP

#include <vector>

#include "entity.hpp"
#include "mob.hpp"

class Object : public Entity {
private:
	int types;
	Mob::Color color;

	Dice dam;
	string name;
	vector<string> description;

	int hit;
	int dodge;
	int def;
	int weight;
	int speed;
	int attribute;
	int value;

public:
	Object(Dungeon *dungeon, Entity::Type type, bool halls);
};

#endif
