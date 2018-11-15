#ifndef MOB_H
#define MOB_H

#include <vector>
#include <deque>
#include <memory>

#include "dice.hpp"
#include "entity.hpp"
#include "object.hpp"

class FMob;
class Mob:  public Entity {
public:
	enum Movement {
		MV_IDLE,
		MV_FAIL,
		MV_SUCCESS,
		MV_DESTROY,
		MV_DAMAGE,
	};

	enum Skills {
		SK_BAD    = 0,
		SK_INTELLIGENCE = (1 << 0),
		SK_TELEPATHY    = (1 << 1),
		SK_TUNNELING    = (1 << 2),
		SK_ERRATIC      = (1 << 3),
		SK_PASS         = (1 << 4),
		SK_PICKUP       = (1 << 5),
		SK_DESTROY      = (1 << 6),
		SK_UNIQUE       = (1 << 7),
		SK_BOSS         = (1 << 8),
	};

	enum Action {
		AC_NONE,
		AC_UP,
		AC_DOWN,
		AC_MOVE,
		AC_QUIT,
		AC_STALL
	};

	enum Pickup {
		PICK_NONE,
		PICK_ADD,
		PICK_WEIGHT,
		PICK_FULL,
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
	Action action = AC_NONE;
	FMob* factory;

	deque<shared_ptr<Object>> inventory;

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
	virtual int getCarryWeight() const;
	Movement move(const Point& next);
	bool isOnEntity(Entity::Type type) const;
	bool isBefore(const Mob& other) const;

	bool isAlive() const {
		return hp > 0;
	}

	int getTurn() const {
		return turn;
	}

	void setTurn(int turn) {
		this->turn = turn;
	}

	void nextTurn() {
		turn += 1000/getSpeed();
	}

	int getOrder() const {
		return order;
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

	string getDamageString() {
		stringstream str;
		str << dam;
		return str.str();
	}
};

#endif
