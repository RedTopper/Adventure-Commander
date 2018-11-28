#include "path.hpp"
#include "dungeon.hpp"

using namespace std;

Path::Path(Dungeon *dungeon, Path::Style style) {
	this->dungeon = dungeon;
	this->style = style;
}

void Path::recalculate() {
	//Reset everything to max
	Point dim = dungeon->getDim();
	this->path = vector<vector<int>>((unsigned long)(dim.y), vector<int>((unsigned long)(dim.x)));
	for (auto& row : path) {
		for (int& num : row) {
			num = INT32_MAX;
		}
	}

	//Create node for player and set distance to player to zero.
	Point play = dungeon->getPlayer()->getPos();
	priority_queue<Priority, vector<Priority>> queue;
	queue.push(Priority{0, play});
	path[play.y][play.x] = 0;

	while (!queue.empty()) {
		Priority cur = queue.top();
		queue.pop();

		int distCurrent = path[cur.p.y][cur.p.x];
		for (const auto &adj : getAdjacent()) {
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

const vector<Point> Path::getAdjacent() {
	vector<Point> adj = {
		{0, -1},
		{1, -1},
		{1,  0},
		{1,  1},
		{0,  1},
		{-1, 1},
		{-1, 0},
		{-1, -1},
	};

	return adj;
}
