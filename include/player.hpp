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
	deque<shared_ptr<Object>> equipped;

private:
	string displayMob(const Mob &other);
	string displayObject(const Object &object);
	void list(const deque<shared_ptr<Object>>& objects, const string& title, int start, void (Player::*action)(int));
	void unequip(int index);
	void equip(int index);
	void expunge(int index);
	void tickInput();
	bool tickScroll(int ch, uint &offset, const string &title, const vector<string> &lines);
	bool tickTarget(int ch, Point& dest);
	bool choice(const vector<string>& text);

public:
	Player(Dungeon* dungeon, WINDOW* window);

	void tick() override;
	int getCarryWeight() const override;
	Mob::Pickup pickUpObject() override;
	static const vector<string> getHelp();

	const deque<shared_ptr<Object>>& getInventory() const {
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
