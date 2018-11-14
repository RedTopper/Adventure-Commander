#ifndef PLAYER_H
#define PLAYER_H

#include <ncursesw/curses.h>

#include "mob.hpp"
#include "main.hpp"

class Player: public Mob {
private:
	WINDOW* base;
	const static Color DEF_COLOR = WHITE;
	const static int DEF_ORDER = 0;
	const static int DEF_SKILLS = 0; //PC has no skills
	const static int DEF_SPEED = 10; //PC has no skills
	const static int DEF_HP = 100;
	vector<shared_ptr<Object>> equipped;

	enum Equip {
		EQ_INVALID,
		EQ_NOT_EQUIPMENT,
		EQ_USED,
		EQ_SUCCESS
	};

private:
	string relative(const Mob& other);
	string item(const Object& object, int index);
	bool tickScroll(int ch, uint &offset, const string &title, const vector<string> &lines);
	bool tickTarget(int ch, Point& dest);
	void tickInput();

public:
	Player(Dungeon* dungeon, WINDOW* window);
	static const vector<string> getHelp();
	void tick() override;
	int getCarryWeight() const override;
	Mob::Pickup pickUpObject() override;
	Player::Equip equip(int item);

	const vector<shared_ptr<Object>>& getInventory() const {
		return inventory;
	}

	int getMaxInventory() const override {
		return 10;
	}

	int getMaxCarryWeight() const override {
		return 40;
	}

	void setDungeon(Dungeon *dungeon) {
		this->dungeon = dungeon;
	}
};

#endif
