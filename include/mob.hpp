#ifndef MOB_H
#define MOB_H

#include <vector>
#include <memory>

#include "dice.hpp"
#include "entity.hpp"
#include "object.hpp"

class FMob;
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

	enum Action {
		NONE,
		UP,
		DOWN,
		MOVE,
		QUIT,
		STALL
	};

	enum Pickup {
		ADD,
		WEIGHT,
		SPACE,
		NOTHING
	};

protected:
	int known;  //How many turns the player is known for
	int order;
	int turn;

	//From Factory
	int skills; //Array of Mob::Skills bits
	int speed;
	int hp;
	Dice dam;

	//For player inputs
	Action action = NONE;
	FMob* factory;

	vector<shared_ptr<Object>> inventory;

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
		FMob* factory,
		Color color,
		int order,
		int skills,
		int speed,
		int hp,
		const Dice& dam,
		const string& name,
		const string& symbol,
		const string& symbolAlt,
		const vector<string>& description
	);

	virtual void tick();
	virtual Pickup pickUpObject();
	Movement move(const Point& next);
	bool isOnEntity(Entity::Type type) const;
	bool isBefore(const Mob& other) const;
	int getCarryWeight() const;

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
		turn += 1000/getSpeed();
	}

	Action getAction() const {
		return action;
	}

	int getHp() const {
		return hp;
	}

	int getSpeed() const {
		return speed;
	}

	virtual int getMaxInventory() const {
		return 1;
	}

	virtual int getMaxCarryWeight() const {
		return 10;
	}
};

#endif
