#include <wchar.h>
#include <stdint.h>
#include <stdlib.h>

#include "types.h"
#include "util.h"

//"Public" functions

int utilSkewedBetweenRange(int min, int max) {
	//75% chance of "upgrading" size.
	int value = min;
	while (rand() % 4 && value < max) value++;
	return value;
}

int utilProdDot(Point one, Point two) {
	two = (Point){
		two.x - one.x,
		two.y - one.y
	};
	
	return (one.x * two.x) + (one.y * two.y); 
}

int utilProdCross(Point one, Point two) {
	two = (Point){
		two.x - one.x,
		two.y - one.y
	};
	
	return (one.x * two.y) - (two.x * one.y); 
}

int utilCompDist(Point one, Point two) {
	int dot = utilProdDot(one, two);
	if(dot == 0) return 0;
	if(dot > 0) return -1;
	return 1;
}

int utilCompAngle(Point one, Point two) {
	int cross = utilProdCross(one, two);
	if(cross == 0) return 0;
	if(cross > 0) return -1;
	return 1;
}

int utilSortRad(const void* pOne, const void* pTwo) {
	Room rOne = *(const Room*) pOne;
	Room rTwo = *(const Room*) pTwo;
	
	Point one = {
		rOne.pos.x + rOne.dim.x/2,
		rOne.pos.y + rOne.dim.y/2,
	};
	
	Point two = {
		rTwo.pos.x + rTwo.dim.x/2,
		rTwo.pos.y + rTwo.dim.y/2,
	};
	
	if(one.x == two.x && one.y == two.y) return 0;
	if(one.x == 0 && one.y == 0) return -1;
	if(two.x == 0 && two.y == 0) return 1;
	if(utilCompAngle(one, two) > 0) return 1;
	if(utilCompAngle(one, two) < 0) return -1;
	if(utilCompDist(one, two) < 0) return -1;
	return 1;
}
