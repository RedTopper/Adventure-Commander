#ifndef ENTITY_H
#define ENTITY_H

#include <array>
#include <memory>
#include <vector>
#include "point.hpp"

using namespace std;

class Dungeon;
class Entity {
public:
	enum Type {
		OTHER,
		STAIRS_UP,
		STAIRS_DOWN
	};

	const string getSymbol() const;
	const string getSymbolAlt() const {
		return this->symbolAlt;
	}

protected:
	bool remembered;
	Dungeon* dungeon;
	Type type;
	Point pos;
	string symbol;
	string symbolAlt;
	string name;
	vector<string> description;
	Color color;

	Point getSpawn(bool halls) const;

public:
	Entity(Dungeon* dungeon, Type type, bool halls);

	const Type& getType() const {
		return type;
	}

	const Point& getPos() const {
		return pos;
	}

	void setPos(const Point& p) {
		pos = p;
	}

	bool isRemembered() const {
		return remembered;
	}

	void setRemembered(bool r) {
		remembered = r;
	}
};

#endif
