#ifndef PLAYER_H
#define PLAYER_H

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
	wstring relative(const Mob& other);
	bool tickMap(int& offset, int ch);

public:
	Player(Dungeon* dungeon, WINDOW* window);
	void tick() override;
	const Action getAction() const {
		return action;
	}
};





#endif
