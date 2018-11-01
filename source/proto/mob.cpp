#include <istream>
#include "proto/mob.hpp"

ProtoMob::KeyWord ProtoMob::toKeyWord(string word) {
	if     (word == "BEGIN") return BEGIN;
	else if(word == "MONSTER") return MONSTER;
	else if(word == "NAME") return NAME;
	else if(word == "DESC") return DESC;
	else if(word == "COLOR") return COLOR;
	else if(word == "SPEED") return SPEED;
	else if(word == "ABIL") return ABIL;
	else if(word == "HP") return HP;
	else if(word == "DAM") return DAM;
	else if(word == "SYMB") return SYMB;
	else if(word == "RRTY") return RRTY;
	else if(word == "END") return END;
	else return BAD_KWD;
}

ProtoMob::Color ProtoMob::toColor(string color) {
	if     (color == "RED") return RED;
	else if(color == "GREEN") return GREEN;
	else if(color == "BLUE") return BLUE;
	else if(color == "CYAN") return CYAN;
	else if(color == "YELLOW") return YELLOW;
	else if(color == "MAGENTA") return MAGENTA;
	else if(color == "WHITE") return WHITE;
	else if(color == "BLACK") return BLACK;
	else return BAD_COLOR;
}

ostream &ProtoMob::dump(ostream &out) const {
	return out;
}

istream &ProtoMob::read(istream &in) {
	string line;
	in >> line;


	return in;
}
