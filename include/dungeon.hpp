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
	vector<vector<int>> pathFllor;
	vector<vector<int>> pathDig;
	wstring status;
	wstring line1;
	wstring line2;
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
	void finalize(int mobs);
	void postProcess();
	int isFull();

public:
	static int skewBetweenRange(int skew, int low, int high);
	Dungeon(const Point& dim, int mobs, int floor, bool emoji);
	Dungeon(ifstream& file, int mobs, bool emoji);
	void save(ofstream& file);
	void print(WINDOW* window);

	//Getters and setters
	const vector<Entity> getEntities() const {
		return vector<Entity>();
	}
	const Player getPlayer() const {
		return *player;
	}
	const Point getDim() const {
		return dim;
	}
	const vector<vector<Tile>> getTiles() const {
		return tiles;
	}
};

#endif
