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
	const static int DEF_HP = 500;
	std::deque<std::shared_ptr<Object>> equipped;

private:
	void list(const std::deque<std::shared_ptr<Object>>& objects, const std::string& title, int start, void (Player::*action)(int));
	std::string displayMob(const Mob &other);
	std::string displayObject(const Object &object);
	void look(Point point);
	void inspect(int index);
	void unequip(int index);
	void equip(int index);
	void expunge(int index);
	void drop(int index);
	void tickInput();
	bool tickScroll(int ch, uint32_t &offset, const std::string &title, const std::vector<std::string> &lines);
	bool tickTarget(int ch, Point &dest);
	bool choice(const std::vector<std::string>& text);
	void attack(const Point& dest) override;

public:
	Player(Dungeon* dungeon, WINDOW* window);
	static const std::vector<std::string> getHelp();
	void flip() const;
	void tick() override;
	int damage(int dam) override;
	int getCarryWeight() const override;
	Mob::Pickup pickUpObject() override;
	Mob::Movement move(const Point& next) override;


	const std::deque<std::shared_ptr<Object>>& getInventory() const {
		return inventory;
	}

	int getMaxInventory() const override {
		return 10;
	}

	int getMaxCarryWeight() const override {
		return 50;
	}

	int getDamage() const override {
		int dam = this->dam.roll();
		for (const auto& o : equipped) dam += o->getDamage().roll();
		return dam;
	}

	int getSpeed() const override {
		int speed = this->speed;
		for (const auto& o : equipped) speed += o->getSpeed();
		return speed > 1 ? speed : 1;
	}

	int getMinDamage() const {
		int min = this->dam.min();
		for (const auto& o : equipped) min += o->getDamage().min();
		return min;
	}

	int getMaxDamage() const {
		int max = this->dam.max();
		for (const auto& o : equipped) max += o->getDamage().max();
		return max;
	}

	int getDefense() const {
		int def = 0;
		for (const auto& o : equipped) def += o->getDef();
		return def;
	}

	void setDungeon(Dungeon *dungeon) {
		this->dungeon = dungeon;
	}
};

#endif
