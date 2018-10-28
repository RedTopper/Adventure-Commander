#ifndef ADVENTURE_COMMANDER_MOB_HPP
#define ADVENTURE_COMMANDER_MOB_HPP

#include <string>
#include "stream.hpp"

using namespace std;

class ProtoMob : public Stream {
public:
	enum KeyWord {
		BEGIN_MONSTER,
		NAME,
		DESC,
		COLOR,
		SPEED,
		ABIL,
		HP,
		DAM,
		SYMB,
		RRTY,
	};

	enum Color {
		RED,
		GREEN,
		BLUE,
		CYAN,
		YELLOW,
		MAGENTA,
		WHITE,
		BLACK
	};

	static KeyWord toKeyWord(string word);
	static Color toColor(string color);
	ostream& dump(ostream& out) const override;
	istream& read(istream& in) override;
};

#endif
