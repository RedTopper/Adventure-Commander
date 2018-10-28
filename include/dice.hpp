#ifndef ADVENTURE_COMMANDER_DICE_HPP
#define ADVENTURE_COMMANDER_DICE_HPP

#include <cstdlib>
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

	ostream& dump(ostream& out) const override;
	istream& read(istream& in) override;
	int roll() const;
};

#endif
