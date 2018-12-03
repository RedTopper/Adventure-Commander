#ifndef DUNGEON_H
#define DUNGEON_H

#include <string>
#include <vector>
#include <queue>
#include <memory>

#include "point.hpp"
#include "tile.hpp"
#include "room.hpp"
#include "path.hpp"

extern const Point DUNGEON_DIM;

class Mob;
class FMob;
class Object;
class FObject;
class Player;
class Entity;
class Driver;
class Order {
public:
	bool operator()(const std::shared_ptr<Mob>& lhs, const std::shared_ptr<Mob>& rhs) const;
};

class Dungeon {
public:
	enum Display {
		NORMAL,
		FOGGY,
		TUNNEL,
		DISTANCE
	};

	static const int FOG_X = 3;
	static const int FOG_Y = 2;

private:
	typedef std::priority_queue<std::shared_ptr<Mob>, std::vector<std::shared_ptr<Mob>>, Order> Turn;

	Point dim;
	std::vector<Room> rooms;
	std::vector<std::shared_ptr<Entity>> entities;
	std::vector<std::shared_ptr<Mob>> mobs;
	std::vector<std::shared_ptr<Object>> objects;
	std::vector<std::vector<Tile>> tiles;
	std::vector<std::vector<Tile>> fog;
	std::shared_ptr<Player> player;
	Turn turn;
	Path map;
	Path dig;
	Display display;
	bool emoji;

	//player snapshot
	Point playerPos;
	int playerTurn;

	template <class F, class T>
	std::vector<std::shared_ptr<T>> generateFactory(std::vector<F>& factories, int total);
	template <class T>
	bool isLeft(const std::shared_ptr<Entity> &original, std::vector<std::shared_ptr<T>> list);
	void connectRoomRasterize(const Point &from, const Point &to);
	void connectRoom(const Room &first, const Room &second);
	bool placeRoomAttempt(const Room &room);
	void placeRoom(const Room &room);
	void placeHall(const Point &point);
	void placeTile(const Point &pos, uint8_t hardness, const float *seed);
	void generateRooms();
	void generateEntities(int floor);
	void printEntity(std::shared_ptr<Driver>& base, const std::shared_ptr<Entity> &e);
	void postProcess(std::vector<std::vector<Tile>>& tiles);
	int isFull();

public:
	std::string status;
	std::string line1;
	std::string line2;

	explicit Dungeon(const Point& dim);
	explicit Dungeon(std::fstream& file);
	void finalize(std::shared_ptr<Driver>& base, std::vector<FMob>& fMob, std::vector<FObject>& fObject, std::shared_ptr<Player>& player, int floor, bool emoji, int count);
	const std::shared_ptr<Mob> getMob(const Point& p);
	void snapshotTake();
	void snapshotRestore();
	void save(std::fstream& file);
	bool alive() const;
	void rotate();
	void updateFoggy();
	void print(std::shared_ptr<Driver>& base);
	bool isInRange(const Entity& e);

	//Getters and setters
	const std::vector<std::shared_ptr<Entity>>& getEntities() const {
		return entities;
	}

	const std::vector<std::shared_ptr<Mob>>& getMobs() const {
		return mobs;
	}

	std::vector<std::shared_ptr<Object>>& getObjects() {
		return objects;
	}

	const std::shared_ptr<Player>& getPlayer() const {
		return player;
	}

	const std::shared_ptr<Mob>& getCurrentTurn() const {
		return turn.top();
	}

	const Point& getDim() const {
		return dim;
	}

	Tile& getTile(Point p) {
		return tiles[p.y][p.x];
	}

	Display getDisplay() const {
		return display;
	}

	bool isFancy() const {
		return emoji;
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

	void setDisplay(const Display display) {
		this->display = display;
	}
};

#endif
