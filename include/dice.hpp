#ifndef ADVENTURE_COMMANDER_STREAM_DICE_HPP
#define ADVENTURE_COMMANDER_STREAM_DICE_HPP

#include <sstream>

#include "stream.hpp"

class Dice : public Stream {
private:
	int base;
	int count;
	int sides;

public:
	Dice() : Dice(0,0, 1) {};
	Dice(int base, int count, int sides);
	int roll() const;
	int min() const;
	int max() const;

private:
	std::ostream& dump(std::ostream& out) const override;
	std::istream& read(std::istream& in) override;
	unsigned int getRequired() const override {
		//No requirements for dice, isValid always true.
		return 0;
	}
};

#endif
