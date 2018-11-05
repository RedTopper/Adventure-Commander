#ifndef MOB_H
#define MOB_H

#include <ncursesw/curses.h>
#include "entity.hpp"

class Mob:  public Entity {
protected:
	int known;
	int skills;
	int speed;
	int order;
	int turn;
	int hp;

public:
	enum Movement {
		IDLE,
		FAILURE,
		SUCCESS,
		BROKE_WALL,
		DAMAGE_WALL,
	};

	enum Skills {
		SKILL_BAD    = 0,
		INTELLIGENCE = (1 << 0),
		TELEPATHY    = (1 << 1),
		TUNNELING    = (1 << 2),
		ERRATIC      = (1 << 3),
		PASS         = (1 << 4),
		PICKUP       = (1 << 5),
		DESTROY      = (1 << 6),
		UNIQUE       = (1 << 7),
		BOSS         = (1 << 8),
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

private:
	Point nextPoint(Point end);
	void statusString(const wstring& text, const wstring& type);
	void tickStraightLine(const wchar_t* type);
	void tickRandomly(const wchar_t* type);
	void tickPathFind(const wchar_t* type);
	bool canSeePC();

protected:
	void attack();

public:
	Mob(Dungeon* dungeon, int turn);
	virtual void tick();
	Movement move(const Point& next);
	const wstring getSymbol() const override;
	bool isOn(Entity::Type type) const;
	bool isBefore(const Mob& other) const;
	bool isAlive() const {
		return hp > 0;
	}
	int getTurn() const {
		return turn;
	}
	int getOrder() const {
		return order;
	}
	void nextTurn() {
		turn += speed;
	}
};

#endif
