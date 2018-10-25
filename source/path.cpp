
#include <path.hpp>

#include "path.hpp"
#include "point.hpp"
#include "path.hpp"
#include "dungeon.hpp"

const Point Path::ADJACENT[8] = { // NOLINT(cert-err58-cpp)
	{0, -1},
	{1, -1},
	{1,  0},
	{1,  1},
	{0,  1},
	{-1, 1},
	{-1, 0},
	{-1, -1},
};

Path::Path(Dungeon *dungeon, Path::Style style) {
	Point dim = dungeon->getDim();
	this->dungeon = dungeon;
	this->style = style;
	this->path = vector<vector<int>>((unsigned long)(dim.y), vector<int>((unsigned long)(dim.x)));
}

void Path::recalculate() {
	//Reset everything to max
	for (auto& row : path) {
		for (int& num : row) {
			num = INT32_MAX;
		}
	}

	//Create node for player and set distance to player to zero.
	priority_queue<Priority, vector<Priority>> queue;
	queue.push(Priority{0, dungeon->getPlayer().getPos()});

	while (!queue.empty()) {
		Priority cur = queue.top();
		queue.pop();

		int distCurrent = path[cur.p.y][cur.p.x];
		for (const auto &adj : ADJACENT) {
			Point next = Point(cur.p);
			next += adj;

			//Make sure nothing bad happens
			if (next < Point() || next >= dungeon->getDim()) continue;

			//Filter out types depending on mode.
			const Tile& tile = dungeon->getTile(next);
			if (tile.type == Tile::VOID || tile.type == Tile::EDGE) continue;
			if (style == VIA_FLOOR && tile.type == Tile::ROCK) continue;

			int nextHardness = tile.hardness;
			int distNextOld = path[next.y][next.x];
			int distNextNew = distCurrent + 1 + (nextHardness / HARDNESS_RATE);

			if (distNextOld > distNextNew) {
				path[next.y][next.x] = distNextNew;
				queue.push(Priority{distNextNew, next});
			}
		}
	}
}
