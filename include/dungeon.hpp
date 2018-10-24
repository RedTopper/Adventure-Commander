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

extern const Point DUNGEON_DIM;

using namespace std;

class Dungeon {
private:
	Point dim;
	shared_ptr<Player> player;
	vector<shared_ptr<Mob>> mobs;
	priority_queue<shared_ptr<Mob>> turn;
	vector<Room> rooms;
	vector<Entity> entities;
	vector<vector<Tile>> tiles;
	vector<vector<int>> pathFloor;
	vector<vector<int>> pathDig;
	bool emoji;
	int floor;

	bool roomPlaceAttempt(const Room& room);
	void roomConnectRasterize(const Point& from, const Point& to);
	void roomConnect(const Room& first, const Room& second);
	void roomPlace(Room room);
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
	void save(ofstream& file);
	int alive() const;
	void print(WINDOW* window);

	//Getters and setters
	const vector<Entity> getEntities() const {
		return entities;
	}
	const vector<shared_ptr<Mob>> getMobs() const {
		return mobs;
	}
	const void setPathFloor(vector<vector<int>>& path) {
		pathFloor = path;
	}
	const void setPathDig(vector<vector<int>>& path) {
		pathDig = path;
	}
	const Player getPlayer() const {
		return *player;
	}
	const Point getDim() const {
		return dim;
	}
	const bool isFancy() const {
		return emoji;
	}
	const int getPathFloor(Point p) const {
		return pathFloor[p.y][p.x];
	}
	const int getPathDig(Point p) const {
		return pathDig[p.y][p.x];
	}
	Tile& getTile(Point p) {
		return tiles[p.y][p.x];
	}
};

#endif
