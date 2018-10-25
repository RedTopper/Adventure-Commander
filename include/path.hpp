#ifndef PATH_H
#define PATH_H

#include <vector>

#include "point.hpp"

using namespace std;

class Dungeon;
class Path {
public:
	static const int HARDNESS_RATE = 85;
	static const Point ADJACENT[8];

	enum Style {
		VIA_DIG,
		VIA_FLOOR
	};

private:
	struct Priority {
		int priority;
		Point p;

		//Max queue by default, we want a min queue
		bool operator<(const Priority& other) const {
			return priority > other.priority;
		}
	};

	Style style;
	Dungeon* dungeon;
	vector<vector<int>> path;

public:
	Path(Dungeon* dungeon, Style style);
	void recalculate();
	int getDist(const Point& p) const {
		return path[p.y][p.x];
	}
};

#endif
