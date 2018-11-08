#include "object.hpp"
#include "stream/fobject.hpp"

Object::Object(
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
) : Entity(
	dungeon,
	OTHER,
	color,
	false
) {
	this->factory = factory;
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