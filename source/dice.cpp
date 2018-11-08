#include <string>

#include "dice.hpp"

Dice::Dice(int base, int count, int sides) {
	this->base = base;
	this->count = count;
	this->sides = sides;
}

ostream &Dice::dump(ostream &out) const {
	return out << "Dice(" << base << "+" << count << "d" << sides << ")";
}

istream &Dice::read(istream &in) {
	in >> base;
	in.ignore(INT32_MAX, '+');
	in >> count;
	in.ignore(INT32_MAX, 'd');
	in >> sides;
	if (sides < 1) sides = 1;
	if (count < 0) count = 0;
	return in;
}

int Dice::roll() const {
	int val = base;
	for (int i = 0; i < count; i++) val += (rand() % sides) + 1;
	return val;
}
