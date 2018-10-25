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
		INTELLIGENCE = 0x01,
		TELEPATHY = 0x02,
		TUNNELING = 0x04,
		ERRATIC = 0x08,
		PC = 0x10,
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
