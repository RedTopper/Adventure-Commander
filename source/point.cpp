#include "point.hpp"

Point::Point() {
	x = 0;
	y = 0;
}

Point::Point(int x, int y) {
	this->x = x;
	this->y = y;
}

Point::Point(const Point& other) {
	x = other.x;
	y = other.y;
}

const Point Point::ZERO = Point(); // NOLINT(cert-err58-cpp)