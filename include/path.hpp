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
		bool operator()(const Priority& lhs, const Priority& rhs) const {
			return lhs.priority < rhs.priority;
		}
	};

	Style style;
	Dungeon* dungeon;
	vector<vector<int>> path;

public:
	Path(Dungeon* dungeon, Style style);
	void recalculate();
	const int getDist(const Point& p) const {
		return path[p.y][p.x];
	}
};

#endif
