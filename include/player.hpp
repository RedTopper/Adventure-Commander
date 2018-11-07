#ifndef PLAYER_H
#define PLAYER_H

#include <curses.h>
#include "mob.hpp"

class Player: public Mob {
public:
	enum Action {
		NONE,
		UP,
		DOWN,
		MOVE,
		QUIT,
		STALL
	};

private:
	WINDOW* base;
	Action action;

private:
	string relative(const Mob& other);
	bool tickMap(int ch, uint& offset);
	bool tickTarget(int ch, Point& dest);

public:
	Player(Dungeon* dungeon, WINDOW* window);
	void tick() override;
	Action getAction() const {
		return action;
	}


};

#endif
