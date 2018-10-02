#ifndef POINT_H
#define POINT_H

typedef struct {
	int x;
	int y;
} Point;

int utilSkewedBetweenRange(int min, int max);
int utilProdDot(Point one, Point two);
int utilProdCross(Point one, Point two);
int utilCompDist(Point one, Point two);
int utilCompAngle(Point one, Point two);
int utilSortRad(const void* pOne, const void* pTwo);

#endif
