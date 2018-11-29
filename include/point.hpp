#ifndef POINT_H
#define POINT_H

class Point {
public:
	int x;
	int y;

	/**
	 * Creates a zero point (0, 0)
	 */
	Point() {
		x = 0;
		y = 0;
	}

	/**
	 * Creates a square (dim, dim)
	 * @param dim The dimensions of the square
	 */
	explicit Point(int dim) {
		this->x = dim;
		this->y = dim;
	}

	/**
	 * Creates a point (x, y)
	 * @param x X pos
	 * @param y Y pos
	 */
	Point(int x, int y) {
		this->x = x;
		this->y = y;
	}

	/**
	 * Copies another point
	 * @param other The point to copy
	 */
	Point(const Point& other) {
		x = other.x;
		y = other.y;
	}

	/**
	 * Sort polar - Answered by StackOverflow user
	 * @param center A center point to compare to
	 * @param b The point to compare to this
	 * @return true if the point b is more counter clockwise via center
	 * @see https://stackoverflow.com/a/6989383
	 * @author ciamej
	 */
	bool less(const Point& center, const Point& b) const {
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

	/**
	 * Checks if both x and y are zero
	 * @return bool true / false
	 */
	inline bool isZero() const {
		return x == 0 && y == 0;
	}

	/**
	 * Adds another point to both x and y of this point
	 * @param other Point to add
	 * @return reference to this with (x + other.x, y + other.y)
	 */
	inline Point& operator+=(const Point& other) {
		x += other.x;
		y += other.y;
		return *this;
	}

	/**
	 * Adds an integer to both values of the point
	 * @param offset An offset
	 * @return reference to this with (x + offset, y + offset)
	 */
	inline Point& operator+=(int offset) {
		x += offset;
		y += offset;
		return *this;
	}

	/**
	 * Adds another point to both x and y of this point
	 * @param other Point to add
	 * @return A point copy with (x + other.x, y + other.y)
	 */
	inline Point operator+(const Point& other) const {
		return Point(*this) += other;
	}

	/**
	 * Adds an integer to both values of the point
	 * @param offset An offset
	 * @return A point copy with (x + offset, y + offset)
	 */
	inline Point operator+(int offset) const {
		return Point(*this) += offset;
	}

	/**
	 * Subtracts another point from both x and y of this point
	 * @param other Point to subtract
	 * @return reference to this with (x - other.x, y - other.y)
	 */
	inline Point& operator-=(const Point& other) {
		x -= other.x;
		y -= other.y;
		return *this;
	}

	/**
	 * Subtracts an integer from both values of the point
	 * @param offset An offset
	 * @return reference to this with (x - offset, y - offset)
	 */
	inline Point& operator-=(int offset) {
		x -= offset;
		y -= offset;
		return *this;
	}

	/**
	 * Subtracts another point from both x and y of this point
	 * @param other Point to subtract
	 * @return A new point with (x - other.x, y - other.y)
	 */
	inline Point operator-(const Point& other) const {
		return Point(*this) -= other;
	}

	/**
	 * Subtracts an integer from both values of the point
	 * @param offset An offset
	 * @return A point copy with (x - offset, y - offset)
	 */
	inline Point operator-(int offset) const {
		return Point(*this) -= offset;
	}

	/**
	 * Checks if a point is exactly another point
	 * @param other The point to check
	 * @return true if x = other.x and y = other.y
	 */
	inline bool operator==(const Point& other) const {
		return x == other.x && y == other.y;
	}

	/**
	 * Checks if a point is not exactly equal to another
	 * @param other The point to check
	 * @return true if x != other.x or y != other.y
	 */
	inline bool operator!=(const Point& other) const {
		return !(*this == other);
	}

	/**
	 * Checks if a point is within the open bounds of another point
	 * @param other The point to check
	 * @return true if x < other.x and y < other.y
	 */
	inline bool operator<(const Point& other) const {
		return x < other.x && y < other.y;
	}

	/**
	 * Like operator< but closed
	 * @param other The point to check
	 * @return true if x <= other.x and y <= other.y
	 */
	inline bool operator<=(const Point& other) const {
		return *this < other || *this == other;
	}

	/**
	 * Checks if a point is greater than the open bounds of another point
	 * @param other The point to check
	 * @return true if x > other.x and y > other.y
	 */
	inline bool operator>(const Point& other) const {
		return x > other.x && y > other.y;
	}

	/**
	 * Like operator> but closed
	 * @param other The point to check
	 * @return true if x >= other.x and y >= other.y
	 */
	inline bool operator>=(const Point& other) const {
		return *this > other || *this == other;
	}


};

#endif
