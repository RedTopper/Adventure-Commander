#ifndef ADVENTURE_COMMANDER_STREAM_DICE_HPP
#define ADVENTURE_COMMANDER_STREAM_DICE_HPP

#include <sstream>

#include "stream.hpp"

using namespace std;

class Dice : public Stream {
private:
	int base;
	int count;
	int sides;

public:
	Dice() : Dice(0,0, 1) {};
	Dice(int base, int count, int sides);
	int roll() const;

private:
	ostream& dump(ostream& out) const override;
	istream& read(istream& in) override;
	int getRequired() const override {
		//No requirements for dice, isValid always true.
		return 0;
	}
};

#endif
