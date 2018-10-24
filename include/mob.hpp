#ifndef MOB_H
#define MOB_H

#include <ncursesw/curses.h>
#include "entity.hpp"

class Mob:  public Entity {
public:
	enum Movement {
		MOVE_FAILURE,
		MOVE_SUCCESS,
		MOVE_BROKE_WALL,
		MOVE_DAMAGE_WALL,
	};

	enum Skills {
		SKILL_INTELLIGENCE = 0x01,
		SKILL_TELEPATHY = 0x02,
		SKILL_TUNNELING = 0x04,
		SKILL_ERRATIC = 0x08,
		SKILL_PC = 0x10,
	};

private:
	int known;
	int skills;
	int speed;
	int order;

public:
	int hp;

	const wstring getSymbol();
	void generateAll(int floor);
	int move(Point destination);
	void tick(WINDOW* base);
};

#endif
