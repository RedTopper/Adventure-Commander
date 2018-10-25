#ifndef DUNGEON_H
#define DUNGEON_H

#include <string>
#include <vector>
#include <queue>
#include <array>
#include <memory>
#include <ncursesw/curses.h>

#include "point.hpp"
#include "tile.hpp"
#include "room.hpp"
#include "entity.hpp"
#include "mob.hpp"
#include "player.hpp"
#include "path.hpp"

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
	Path map;
	Path dig;
	bool emoji;
	int floor;

	bool roomPlaceAttempt(const Room& room);
	void roomConnectRasterize(const Point& from, const Point& to);
	void roomConnect(const Room& first, const Room& second);
	void roomPlace(const Room& room);
	void roomGenerate();
	void hallPlace(const Point& point);
	void tilePlace(const Point &pos, uint8_t hardness, const float* seed);
	void entityGenerate();
	void mobGenerate(int total);
	void finalize(int mobs);
	void postProcess();
	int isFull();

public:
	wstring status;
	wstring line1;
	wstring line2;

	Dungeon(WINDOW* base, const Point& dim, int mobs, int floor, bool emoji);
	Dungeon(WINDOW* base, fstream& file, int mobs, bool emoji);
	void save(fstream& file);
	int alive() const;
	void rotate();
	void print(WINDOW* window);

	//Getters and setters
	const vector<Entity> getEntities() const {
		return entities;
	}
	const vector<shared_ptr<Mob>> getMobs() const {
		return mobs;
	}
	const shared_ptr<Player> getPlayer() const {
		return player;
	}
	const Point getDim() const {
		return dim;
	}
	const bool isFancy() const {
		return emoji;
	}
	const int getPathMap(const Point& p) const {
		return map.getDist(p);
	}
	const int getPathDig(const Point& p) const {
		return dig.getDist(p);
	}
	const shared_ptr<Mob> getCurrentTurn() const {
		return turn.top();
	}
	void recalculate() {
		dig.recalculate();
		map.recalculate();
	}
	Tile& getTile(Point p) {
		return tiles[p.y][p.x];
	}
};

#endif
