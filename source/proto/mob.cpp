#include "proto/mob.hpp"

ProtoMob::KeyWord ProtoMob::toKeyWord(string word) {
	return COLOR;
}

ProtoMob::Color ProtoMob::toColor(string color) {
	return MAGENTA;
}

ostream &ProtoMob::dump(ostream &out) const {
	return <#initializer#>;
}

istream &ProtoMob::read(istream &in) {
	return <#initializer#>;
}
