#ifndef ENTITY_H
#define ENTITY_H

#include <array>
#include <memory>
#include "point.hpp"

using namespace std;

class Dungeon;
class Entity {
public:
	enum Type {
		MOB,
		STAIRS_UP,
		STAIRS_DOWN
	};

	virtual const string getSymbol() const;

protected:
	Dungeon* dungeon;
	Type type;
	Point pos;
	bool remembered;
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
