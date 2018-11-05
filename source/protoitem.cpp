#include <iostream>
#include <bitset>

#include "protomob.hpp"
#include "main.hpp"
#include "protoitem.hpp"

ProtoItem::KeyWord ProtoItem::toKeyWord(string word) {
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

ProtoItem::Type ProtoItem::toType(string word) {
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

ostream &ProtoItem::dump(ostream &out) const {
	return out;
}

istream &ProtoItem::read(istream &in) {
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
				while(!!(line >> word)) colors |= ProtoMob::toColor(word);
				break;
			case DESC:
				while(!!(getline(in, buff)) && trim(word = buff) != ".") description.push_back(buff);
				break;
			default:
				cout << "[ERROR]: There was a problem reading a monster!" << endl;
				cout << "[ERROR]: Set bits are " << bitset<32>(keywords) << " but should be " << bitset<32>(REQUIRED) << endl;
				/* FALLTHROUGH */
			case END:
				reading = false;
				break;
		}
	}

	return in;
}

