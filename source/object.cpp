#include "object.hpp"
#include "fobject.hpp"

using namespace std;

Object::Object(
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
	const string& name,
	const string& symbol,
	const string& symbolAlt,
	const vector<string>& description
) : Entity(
	dungeon,
	OTHER,
	color,
	false
) {
	this->types = types;
	this->hit = hit;
	this->dodge = dodge;
	this->def = def;
	this->weight = weight;
	this->speed = speed;
	this->attribute = attribute;
	this->value = value;
	this->dam = dam;
	this->name = name;
	this->symbol = symbol;
	this->symbolAlt = symbolAlt;
	this->description = description;
}


bool Object::isEquipment() const {
	int equipment = (
		FObject::WEAPON
		| FObject::OFFHAND
		| FObject::RANGED
		| FObject::ARMOR
		| FObject::HELMET
		| FObject::CLOAK
		| FObject::GLOVES
		| FObject::BOOTS
		| FObject::RING
		| FObject::AMULET
		| FObject::LIGHT
	);

	//Checks if all "types" bits are in "equipment" bits
	return (types | equipment) == equipment;
}