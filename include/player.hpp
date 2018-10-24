#ifndef PLAYER_H
#define PLAYER_H

#include "mob.hpp"

class Player: public Mob {
public:
	enum Action {
		ACTION_NONE,
		ACTION_UP,
		ACTION_DOWN,
		ACTION_MOVE,
		ACTION_QUIT,
		ACTION_STALL
	};

	Action playerAction(Mob* mob, Dungeon* dungeon, WINDOW* base);
	void tick(WINDOW* base);
};





#endif
