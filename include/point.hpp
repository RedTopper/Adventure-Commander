#ifndef POINT_H
#define POINT_H

#include <ctgmath>

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

	inline bool isClockwise(const Point& other) const {
		return atan2(*this^other, *this*other) > 0;
	}

	inline bool isZero() const {
		return x == 0 && y == 0;
	}

	//The dot product
	inline int operator*(const Point& other) const {
		return x * other.x + y * other.y;
	}

	//The cross product, or determinant
	inline int operator^(const Point& other) const {
		return x * other.y - y * other.x;
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
