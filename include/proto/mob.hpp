#ifndef ADVENTURE_COMMANDER_MOB_HPP
#define ADVENTURE_COMMANDER_MOB_HPP

#include <string>
#include "stream.hpp"

using namespace std;

class ProtoMob : public Stream {
public:
	enum KeyWord {
		BAD_KWD,
		BEGIN,
		MONSTER,
		NAME,
		DESC,
		COLOR,
		SPEED,
		ABIL,
		HP,
		DAM,
		SYMB,
		RRTY,
		END,
	};

	enum Color {
		BAD_COLOR = 0,
		RED     = (1 << 0),
		GREEN   = (1 << 1),
		BLUE    = (1 << 2),
		CYAN    = (1 << 3),
		YELLOW  = (1 << 4),
		MAGENTA = (1 << 5),
		WHITE   = (1 << 6),
		BLACK   = (1 << 7)
	};

	static KeyWord toKeyWord(string word);
	static Color toColor(string color);
	ostream& dump(ostream& out) const override;
	istream& read(istream& in) override;
};

#endif
