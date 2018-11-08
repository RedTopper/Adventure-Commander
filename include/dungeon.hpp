#ifndef DUNGEON_H
#define DUNGEON_H

#include <ncursesw/curses.h>
#include <string>
#include <vector>
#include <queue>
#include <array>
#include <memory>

#include "point.hpp"
#include "tile.hpp"
#include "room.hpp"
#include "entity.hpp"
#include "mob.hpp"
#include "player.hpp"
#include "path.hpp"
#include "stream/fmob.hpp"
#include "stream/fobject.hpp"

extern const Point DUNGEON_DIM;

using namespace std;

class Dungeon {
private:
	struct TurnOrder {
		//Should return true if lhs is considered to go before rhs
		bool operator()(const shared_ptr<Mob>& lhs, const shared_ptr<Mob>& rhs) const {
			return !lhs->isBefore(*rhs);
		}
	};

	Point dim;
	shared_ptr<Player> player;
	vector<shared_ptr<Mob>> mobs;
	priority_queue<shared_ptr<Mob>, vector<shared_ptr<Mob>>, TurnOrder> turn;
	vector<Room> rooms;
	vector<Entity> entities;
	vector<vector<Tile>> tiles;
	vector<vector<Tile>> fog;
	Path map;
	Path dig;
	bool emoji;
	bool foggy;

public:
	static const int FOG_X = 3;
	static const int FOG_Y = 2;

private:
	bool roomPlaceAttempt(const Room& room);
	void roomConnectRasterize(const Point& from, const Point& to);
	void roomConnect(const Room& first, const Room& second);
	void roomPlace(const Room& room);
	void roomGenerate();
	void hallPlace(const Point& point);
	void tilePlace(const Point &pos, uint8_t hardness, const float* seed);
	void entityGenerate(int floor);
	void mobGenerate(vector<FMob> factoryMob, int total);
	void printMob(WINDOW *win, const shared_ptr<Mob> &m);
	void postProcess(vector<vector<Tile>>& tiles);
	int isFull();

public:
	string status;
	string line1;
	string line2;

	Dungeon(WINDOW* base, const Point& dim);
	Dungeon(WINDOW* base, fstream& file);
	void finalize(vector<FMob>& fMob, vector<FObject>& fObject, int count,  int floor, bool emoji);
	void save(fstream& file);
	int alive() const;
	void rotate();
	void updateFoggy();
	void print(WINDOW* window);

	//Getters and setters
	const vector<Entity>& getEntities() const {
		return entities;
	}

	const vector<shared_ptr<Mob>>& getMobs() const {
		return mobs;
	}

	const shared_ptr<Player>& getPlayer() const {
		return player;
	}

	const shared_ptr<Mob>& getCurrentTurn() const {
		return turn.top();
	}

	const Point& getDim() const {
		return dim;
	}

	Tile& getTile(Point p) {
		return tiles[p.y][p.x];
	}
	bool isFancy() const {
		return emoji;
	}

	bool isFoggy() const {
		return foggy;
	}

	bool isOutOfRange(const Entity& e) {
		return foggy && !e.isRemembered() && player
			&& (e.getPos().x - player->getPos().x < -FOG_X
			|| e.getPos().x - player->getPos().x > FOG_X
			|| e.getPos().y - player->getPos().y < -FOG_Y
			|| e.getPos().y - player->getPos().y > FOG_Y);
	}

	int getPathMap(const Point& p) const {
		return map.getDist(p);
	}

	int getPathDig(const Point& p) const {
		return dig.getDist(p);
	}

	void recalculate() {
		dig.recalculate();
		map.recalculate();
	}

	void setFoggy(const bool foggy) {
		this->foggy = foggy;
	}
};

#endif
