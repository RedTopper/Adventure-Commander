#ifndef PLAYER_H
#define PLAYER_H

#include <ncursesw/curses.h>

#include "mob.hpp"

class Player: public Mob {
private:
	WINDOW* base;
	const static Color DEF_COLOR = WHITE;
	const static int DEF_ORDER = 0;
	const static int DEF_SKILLS = 0; //PC has no skills
	const static int DEF_SPEED = 10; //PC has no skills
	const static int DEF_HP = 100;

private:
	string relative(const Mob& other);
	bool tickMap(int ch, uint& offset);
	bool tickTarget(int ch, Point& dest);
	void tickInput();

public:
	Player(Dungeon* dungeon, WINDOW* window);
	Mob::Pickup pickUpObject() override;
	void tick() override;

	const vector<shared_ptr<Object>>& getInventory() const {
		return inventory;
	}

	int getMaxInventory() const override {
		return 10;
	}

	int getMaxCarryWeight() const override {
		return 40;
	}
};

#endif
