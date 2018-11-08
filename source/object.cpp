#include "object.hpp"

Object::Object(
	Dungeon *dungeon,
	const Dice& dam,
	const string& name,
	const string& symbol,
	const string& symbolAlt,
	const vector<string>& description
) : Entity(
	dungeon,
	OTHER,
	false
) {

}
