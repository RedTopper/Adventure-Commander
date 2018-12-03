#ifndef ENTITY_H
#define ENTITY_H

#include <vector>
#include <string>

#include "point.hpp"

class Dungeon;
class Entity {
public:
	enum Type {
		OTHER,
		STAIRS_UP,
		STAIRS_DOWN
	};

	enum Color {
		COLOR_BAD = 0,
		RED     = (1 << 0),
		GREEN   = (1 << 1),
		BLUE    = (1 << 2),
		CYAN    = (1 << 3),
		YELLOW  = (1 << 4),
		MAGENTA = (1 << 5),
		WHITE   = (1 << 6),
		BLACK   = (1 << 7)
	};

protected:
	bool remembered;
	Dungeon* dungeon;
	Type type;
	Point pos;
	Color color;
	std::string name;
	std::string symbol;
	std::string symbolAlt;
	std::vector<std::string> description;

	Point getSpawn(bool halls) const;

public:
	Entity(Dungeon* dungeon, Type type, Color color, bool halls);
	virtual ~Entity() = default;

	void setRemembered(bool r);
	const std::string getSymbol() const;

	const std::string getSymbolAlt() const {
		return this->symbolAlt;
	}

	const Type& getEntityType() const {
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

	Color getColor() const {
		return color;
	}

	const std::string& getName() const {
		return name;
	}

	const std::vector<std::string>& getDescription() const {
		return description;
	}
};

#endif
