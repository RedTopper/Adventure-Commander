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
	int hp;

	Point nextPoint(Point end);
	Movement move(Point& next);
	void statusString(const wstring& text, const wstring& type);
	void tickStraightLine(const wchar_t* type);
	void tickRandomly(const wchar_t* type);
	void tickPathFind(const wchar_t* type);
	bool canSeePC();

public:
	Mob(Dungeon* dungeon, int turn);
	virtual const wstring getSymbol();
	virtual const Point getSpawn();
	virtual void tick();
	bool isAlive() {
		return hp > 0;
	}
};

#endif
