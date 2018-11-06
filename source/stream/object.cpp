#include <iostream>
#include <bitset>

#include "main.hpp"
#include "stream/monster.hpp"
#include "stream/object.hpp"

StreamItem::KeyWord StreamItem::toKeyWord(string word) {
	if(trim(word).empty()) return KWD_EMPTY;
	else if(word == "BEGIN") return BEGIN;
	else if(word == "OBJECT") return OBJECT;
	else if(word == "NAME") return NAME;
	else if(word == "DESC") return DESC;
	else if(word == "TYPE") return TYPE;
	else if(word == "COLOR") return COLOR;
	else if(word == "HIT") return HIT;
	else if(word == "DAM") return DAM;
	else if(word == "DODGE") return DODGE;
	else if(word == "DEF") return DEF;
	else if(word == "WEIGHT") return WEIGHT;
	else if(word == "SPEED") return SPEED;
	else if(word == "ATTR") return ATTR;
	else if(word == "VAL") return VAL;
	else if(word == "ART") return ART;
	else if(word == "RRTY") return RRTY;
	else if(word == "END") return END;
	else return KWD_BAD;
}

StreamItem::Type StreamItem::toType(string word) {
	if     (word == "WEAPON") return WEAPON;
	else if(word == "OFFHAND") return OFFHAND;
	else if(word == "RANGED") return RANGED;
	else if(word == "ARMOR") return ARMOR;
	else if(word == "HELMET") return HELMET;
	else if(word == "CLOAK") return CLOAK;
	else if(word == "GLOVES") return GLOVES;
	else if(word == "BOOTS") return BOOTS;
	else if(word == "RING") return RING;
	else if(word == "AMULET") return AMULET;
	else if(word == "LIGHT") return LIGHT;
	else if(word == "SCROLL") return SCROLL;
	else if(word == "BOOK") return BOOK;
	else if(word == "FLASK") return FLASK;
	else if(word == "GOLD") return GOLD;
	else if(word == "AMMUNITION") return AMMUNITION;
	else if(word == "FOOD") return FOOD;
	else if(word == "WAND") return WAND;
	else if(word == "CONTAINER") return CONTAINER;
	else return TYPE_BAD;
}

ostream &StreamItem::dump(ostream &out) const {
	out << "BEGIN PARSED OBJECT" << endl;
	out << "NAME:  '" << name << "'" << endl;
	out << "DESC:" << endl;
	for(const auto& line : description) cout << line << endl;
	out << "TYPE:  '" << bitset<32>(types) << "'" << endl;
	out << "COLOR: '" << bitset<16>(colors) << "'" << endl;
	out << "HIT:   '" << hit << "'" <<  endl;
	out << "DAM:   '" << dam << "'" << endl;
	out << "DODGE: '" << dodge << "'" << endl;
	out << "DEF:   '" << def << "'" << endl;
	out << "WEIGH: '" << weight << "'" << endl;
	out << "SPEED: '" << speed << "'" << endl;
	out << "ATTR:  '" << attribute << "'" << endl;
	out << "VAL:   '" << value << "'" << endl;
	out << "ART:   '" << artifact << "'" << endl;
	out << "RRTY:  '" << rarity << "'" << endl;
	out << "END" << endl << endl;
	return out;
}

istream &StreamItem::read(istream &in) {
	string header;
	in >> header;
	if (toKeyWord(header) != BEGIN) return in;
	in >> header;
	if (toKeyWord(header) != OBJECT) return in;

	//consume rest of line
	getline(in, header);
	bool reading = true;
	while (!!in && reading) {
		string buff;
		string word;
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
			case DESC:
				while(!!(getline(in, buff)) && trim(word = buff) != ".") description.push_back(buff);
				break;
			case TYPE:
				while(!!(line >> word)) types |= toType(word);
				break;
			case COLOR:
				while(!!(line >> word)) colors |= StreamMob::toColor(word);
				break;
			case HIT:
				line >> hit;
				break;
			case DAM:
				line >> dam;
				break;
			case DODGE:
				line >> dodge;
				break;
			case DEF:
				line >> def;
				break;
			case WEIGHT:
				line >> weight;
				break;
			case SPEED:
				line >> speed;
				break;
			case ATTR:
				line >> attribute;
				break;
			case VAL:
				line >> value;
				break;
			case ART:
				line >> word;
				artifact = word == "TRUE";
				break;
			case RRTY:
				line >> rarity;
				break;
			default:
				cout << "[ERROR]: There was a problem reading a monster!" << endl;
				cout << "[ERROR]: Set bits are " << bitset<32>(keywords) << " but should be " << bitset<32>(getRequired()) << endl;
				/* FALLTHROUGH */
			case END:
				reading = false;
				break;
		}
	}

	return in;
}

int StreamItem::getRequired() const {
	cout << "TYPE:  '" << bitset<32>(types) << "'" << endl;
	return (
		  NAME
		| DESC
		| TYPE
		| COLOR
		| HIT
		| DAM
		| DODGE
		| DEF
		| WEIGHT
		| SPEED
		| ATTR
		| VAL
		| ART
		| RRTY
		| END
	);
}

bool StreamItem::isEquipment() const {
	int equipment = (
		  WEAPON
		| OFFHAND
		| RANGED
		| ARMOR
		| HELMET
		| CLOAK
		| GLOVES
		| BOOTS
		| RING
		| AMULET
		| LIGHT
	);

	//Checks if all "types" bits are in "equipment" bits
	return (types | equipment) == equipment;
}
