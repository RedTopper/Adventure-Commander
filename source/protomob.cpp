#include <istream>
#include <sstream>
#include <iostream>
#include <bitset>
#include "main.hpp"
#include "mob.hpp"
#include "protomob.hpp"

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

Mob::Color ProtoMob::toColor(string color) {
	if     (color == "RED") return Mob::RED;
	else if(color == "GREEN") return Mob::GREEN;
	else if(color == "BLUE") return Mob::BLUE;
	else if(color == "CYAN") return Mob::CYAN;
	else if(color == "YELLOW") return Mob::YELLOW;
	else if(color == "MAGENTA") return Mob::MAGENTA;
	else if(color == "WHITE") return Mob::WHITE;
	else if(color == "BLACK") return Mob::BLACK;
	else return Mob::BAD_COLOR;
}

Mob::Skills ProtoMob::toSkill(string skill) {
	if     (skill == "SMART") return Mob::INTELLIGENCE;
	else if(skill == "TELE") return Mob::TELEPATHY;
	else if(skill == "TUNNEL") return Mob::TUNNELING;
	else if(skill == "ERRATIC") return Mob::ERRATIC;
	else if(skill == "PASS") return Mob::PASS;
	else if(skill == "PICKUP") return Mob::PICKUP;
	else if(skill == "DESTROY") return Mob::DESTROY;
	else if(skill == "UNIQ") return Mob::UNIQUE;
	else if(skill == "BOSS") return Mob::BOSS;
	else return Mob::BAD_SKILL;
}

void ProtoMob::invalidate(istream& in) {
	valid = false;
	cerr << "[ERROR]: There was a problem reading a monster!" << endl;
	cerr << "[ERROR]: Attempting to recover..." << endl;
	string end;
	while (!!in && toKeyWord(end) != END) {
		getline(in, end);
	}
}

ostream &ProtoMob::dump(ostream &out) const {
	out << "BEGIN PARSED MONSTER" << endl;
	out << "NAME:  '" << name << "'" << endl;
	out << "DESC:" << endl;
	for(const auto& line : description) cout << line << endl;
	out << "COLOR: '" << bitset<8>(colors) << "'" << endl;
	out << "SPEED: '" << speed << endl;
	out << "ABIL:  '" << bitset<16>(abilities) << "'" << endl;
	out << "HP:    '" << hp << "'" << endl;
	out << "DAM:   '" << dam << "'" << endl;
	out << "SYMB:  '" << symbol << "'" << endl;
	out << "END" << endl << endl;
	return out;
}

istream &ProtoMob::read(istream &in) {

	string header;
	in >> header;
	if (toKeyWord(header) != BEGIN) return in;
	in >> header;
	if (toKeyWord(header) != MONSTER) return in;

	//consume rest of line
	getline(in, header);
	valid = true;
	bool reading = true;
	while (!!in && reading && valid) {
		string buff;
		string word;
		getline(in, buff);
		stringstream line(buff);
		line >> word;
		switch (toKeyWord(word)) {
			case NAME:
				getline(line, name);
				trim(name);
				break;
			case SYMB:
				line >> symbol;
				break;
			case HP:
				line >> hp;
				break;
			case DAM:
				line >> dam;
				break;
			case SPEED:
				line >> speed;
				break;
			case RRTY:
				line >> rarity;
				break;
			case COLOR:
				while(!!(line >> word)) colors |= toColor(word);
				break;
			case ABIL:
				while(!!(line >> word)) abilities |= toSkill(word);
				break;
			case DESC:
				while(!!(getline(in, buff)) && trim(word = buff) != ".") description.push_back(buff);
				break;
			case END:
				reading = false;
				break;
			default:
			case BEGIN:
			case BAD_KWD:
				invalidate(in);
				break;
		}
	}

	return in;
}



