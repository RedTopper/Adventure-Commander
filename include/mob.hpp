#ifndef MOB_H
#define MOB_H

#include <vector>

#include "stream/dice.hpp"
#include "entity.hpp"

class Mob:  public Entity {
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

protected:
	int known;  //How many turns the player is known for
	int order;
	int turn;

	//From Factory
	int skills; //Array of Mob::Skills bits
	int speed;
	int hp;
	string name;
	string symbol;
	vector<string> description;
	Color color;
	Dice dam;

private:
	Point nextPoint(Point end);
	void statusString(const string& text, const string& type);
	void tickStraightLine(const string& type);
	void tickRandomly(const string& type);
	void tickPathFind(const string& type);
	bool canSeePC();

protected:
	void attack();

public:
	Mob(
		Dungeon* dungeon,
		int turn,
		int skills,
		int speed,
		int hp,
		Color color,
		const string& name,
		const string& symbol,
		const vector<string>& description
	);

	virtual void tick();
	Movement move(const Point& next);
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
	const string getSymbol() const override {
		return symbol;
	};
};

#endif
