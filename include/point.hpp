#ifndef POINT_H
#define POINT_H

class Point {
public:
	int x;
	int y;

	Point() {
		x = 0;
		y = 0;
	}

	Point(int x, int y) {
		this->x = x;
		this->y = y;
	}

	Point(const Point& other) {
		x = other.x;
		y = other.y;
	}

	// Sort polar - Answered by StackOverflow user ciamej
	// https://stackoverflow.com/a/6989383
	inline bool less(const Point& center, const Point& b) const {
		Point a = *this;

		if (a.x - center.x >= 0 && b.x - center.x < 0) return true;
		if (a.x - center.x < 0 && b.x - center.x >= 0) return false;
		if (a.x - center.x == 0 && b.x - center.x == 0) {
			if (a.y - center.y >= 0 || b.y - center.y >= 0) return a.y > b.y;
			return b.y > a.y;
		}

		// compute the cross product of vectors (center -> a) x (center -> b)
		int det = (a.x - center.x) * (b.y - center.y) - (b.x - center.x) * (a.y - center.y);
		if (det < 0) return true;
		if (det > 0) return false;

		// points a and b are on the same line from the center
		// check which point is closer to the center
		int d1 = (a.x - center.x) * (a.x - center.x) + (a.y - center.y) * (a.y - center.y);
		int d2 = (b.x - center.x) * (b.x - center.x) + (b.y - center.y) * (b.y - center.y);
		return d1 > d2;
	}

	inline bool isZero() const {
		return x == 0 && y == 0;
	}

	inline Point &operator+=(const Point& other) {
		x += other.x;
		y += other.y;
		return *this;
	}

	inline Point &operator-=(const Point& other) {
		x -= other.x;
		y -= other.y;
		return *this;
	}

	inline bool operator<(const Point& other) const {
		return x < other.x && y < other.y;
	}

	inline bool operator==(const Point& other) const {
		return x == other.x && y == other.y;
	}

	//Defined in terms of += and -=
	inline Point operator+(const Point& other) {
		return Point(*this) += other;
	}

	inline Point operator-(const Point& other) {
		return Point(*this) -= other;
	}

	//Defined in terms of < and ==
	inline bool operator<=(const Point& other) const {
		return *this < other || *this == other;
	}

	inline bool operator>(const Point& other) const {
		return !(*this < other || *this == other);
	}

	inline bool operator>=(const Point& other) const {
		return !(*this < other);
	}

	inline bool operator!=(const Point& other) const {
		return !(*this == other);
	}
};

#endif
