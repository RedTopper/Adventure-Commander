#include <istream>
#include <sstream>
#include <iostream>
#include <bitset>

#include "main.hpp"
#include "fmob.hpp"

using namespace std;

FMob::KeyWord FMob::toKeyWord(string word) {
	if(trim(word).empty()) return KWD_EMPTY;
	else if(word == "BEGIN") return BEGIN;
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
	else return KWD_BAD;
}

Entity::Color FMob::toColor(string color) {
	if     (color == "RED") return Entity::RED;
	else if(color == "GREEN") return Entity::GREEN;
	else if(color == "BLUE") return Entity::BLUE;
	else if(color == "CYAN") return Entity::CYAN;
	else if(color == "YELLOW") return Entity::YELLOW;
	else if(color == "MAGENTA") return Entity::MAGENTA;
	else if(color == "WHITE") return Entity::WHITE;
	else if(color == "BLACK") return Entity::BLACK;
	else return Entity::COLOR_BAD;
}

Mob::Skills FMob::toSkill(string skill) {
	if     (skill == "SMART") return Mob::SK_INTELLIGENCE;
	else if(skill == "TELE") return Mob::SK_TELEPATHY;
	else if(skill == "TUNNEL") return Mob::SK_TUNNELING;
	else if(skill == "ERRATIC") return Mob::SK_ERRATIC;
	else if(skill == "PASS") return Mob::SK_PASS;
	else if(skill == "PICKUP") return Mob::SK_PICKUP;
	else if(skill == "DESTROY") return Mob::SK_DESTROY;
	else if(skill == "UNIQ") return Mob::SK_UNIQUE;
	else if(skill == "BOSS") return Mob::SK_BOSS;
	else return Mob::SK_BAD;
}

ostream& FMob::dump(ostream& out) const {
	out << "BEGIN PARSED MONSTER" << endl;
	out << "NAME:  '" << name << "'" << endl;
	out << "DESC:" << endl;
	for(const auto& line : description) cout << line << endl;
	out << "COLOR: '" << bitset<16>(colors) << "'" << endl;
	out << "SPEED: '" << speed << endl;
	out << "ABIL:  '" << bitset<16>(abilities) << "'" << endl;
	out << "HP:    '" << hp << "'" << endl;
	out << "DAM:   '" << dam << "'" << endl;
	out << "SYMB:  '" << symbol << "'" << endl;
	out << "END" << endl << endl;
	return out;
}

istream& FMob::read(istream& in) {
	string header;
	in >> header;
	if (toKeyWord(header) != BEGIN) return in;
	in >> header;
	if (toKeyWord(header) != MONSTER) return in;

	//consume rest of line
	getline(in, header);
	bool reading = true;
	while (!!in && reading) {
		string buff, word;
		getline(in, buff);
		stringstream line(buff);
		line >> word;
		KeyWord key = toKeyWord(word);
		keywords |= key;
		switch (key) {
			case KWD_EMPTY:
				break;
			case NAME:
				getline(line, name);
				trim(name);
				break;
			case SYMB:
				line >> word;
				symbolAlt = word.substr(0, 1);
				symbol = word.substr(1);
				if (symbol.empty()) symbol = symbolAlt;
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
				while(!!(line >> word)) {
					Mob::Skills skill = toSkill(word);
					abilities |= skill;
					if (Mob::Skills::SK_UNIQUE & skill) unique = true;
				}
				break;
			case DESC:
				while(!!(getline(in, buff)) && trim(word = buff) != ".") description.push_back(buff);
				break;
			default:
				cout << "[ERROR]: There was a problem reading a monster!" << endl;
				cout << "[ERROR]: Set bits are " << bitset<16>(keywords) << " but should be " << bitset<16>(getRequired()) << endl;
				/* FALLTHROUGH */
			case END:
				reading = false;
				break;
		}
	}

	return in;
}

unsigned int FMob::getRequired() const {
	return (
		  NAME
		| DESC
		| COLOR
		| SPEED
		| ABIL
		| HP
		| DAM
		| SYMB
		| RRTY
		| END
	);
}

const shared_ptr<Entity> FMob::make(Dungeon *dungeon, int turn) {
	return make_shared<Mob>(
		dungeon,
		this,
		getRandomColor(colors),
		turn,
		abilities,
		speed.roll(),
		hp.roll(),
		dam,
		name,
		symbol,
		symbolAlt,
		description
	);
}



