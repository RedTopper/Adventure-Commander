#include <iostream>
#include <bitset>
#include <array>

#include "main.hpp"
#include "fmob.hpp"
#include "fobject.hpp"
#include "object.hpp"

using namespace std;

FObject::KeyWord FObject::toKeyWord(string word) {
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

FObject::Type FObject::toType(string word) {
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

string FObject::fromType(int type) {
	string str;
	if(type & WEAPON) str += "Weapon, ";
	if(type & OFFHAND) str +=  "Off-Hand, ";
	if(type & RANGED) str +=  "Ranged, ";
	if(type & ARMOR) str +=  "Armor, ";
	if(type & HELMET) str +=  "Helmet, ";
	if(type & CLOAK) str +=  "Cloak, ";
	if(type & GLOVES) str +=  "Gloves, ";
	if(type & BOOTS) str +=  "Boots, ";
	if(type & RING) str +=  "Ring, ";
	if(type & AMULET) str +=  "Amulet, ";
	if(type & LIGHT) str +=  "Light, ";
	if(type & SCROLL) str +=  "Scroll, ";
	if(type & BOOK) str +=  "Book, ";
	if(type & FLASK) str +=  "Flask, ";
	if(type & GOLD) str +=  "Gold, ";
	if(type & AMMUNITION) str +=  "Ammo, ";
	if(type & FOOD) str +=  "Food, ";
	if(type & WAND) str +=  "Wand, ";
	if(type & CONTAINER) str +=  "Container, ";
	if(str.length() < 2) return str;
	return str.erase(str.length() - 2);
}

ostream &FObject::dump(ostream &out) const {
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
	out << "ART:   '" << (unique ? "yeah" : "nah") << "'" << endl;
	out << "RRTY:  '" << rarity << "'" << endl;
	out << "END" << endl << endl;
	return out;
}

istream &FObject::read(istream &in) {
	string header;
	in >> header;
	if (toKeyWord(header) != BEGIN) return in;
	in >> header;
	if (toKeyWord(header) != OBJECT) return in;

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
			case DESC:
				while(!!(getline(in, buff)) && trim(word = buff) != ".") description.push_back(buff);
				break;
			case TYPE:
				while(!!(line >> word)) types |= toType(word);
				break;
			case COLOR:
				while(!!(line >> word)) colors |= FMob::toColor(word);
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
				unique = word == "TRUE";
				break;
			case RRTY:
				line >> rarity;
				break;
			default:
				cout << "[ERROR]: There was a problem reading an object!" << endl;
				cout << "[ERROR]: Set bits are " << bitset<32>(keywords) << " but should be " << bitset<32>(getRequired()) << endl;
				/* FALLTHROUGH */
			case END:
				reading = false;
				break;
		}
	}

	return in;
}

unsigned int FObject::getRequired() const {
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

const shared_ptr<Entity> FObject::make(Dungeon *dungeon, int) {
	const array<string, 20> SYMBOL = {
		u8"\U0001F531", //Trident
		u8"\U0001F52A", //Knife
		u8"\U0001F3F9", //Bow
		u8"\U0001F530", //Shield
		u8"\U0001F3A9", //Top Hat
		u8"\U0001F455", //Shirt
		u8"\U0001F9E4", //Gloves
		u8"\U0001F45E", //Shoes
		u8"\U0001F48D", //Ring
		u8"\U0001F4FF", //Prayer Beads
		u8"\U0001F526", //Flashlight
		u8"\U0001F4DC", //Scroll
		u8"\U0001F4D5", //Book
		u8"\U0001F378", //Cocktail
		u8"\U0001F4B0", //Money Bag
		u8".",          //Ammo
		u8"\U0001F372", //Food
		u8"\U0001F31F", //Star
		u8"\U0001F4E6", //Package
		u8"*"
	};

	const array<string, 20> SYMBOL_BORING = {
		"|", //WEAPON
		")", //OFFHAND
		"}", //RANGED
		"[", //ARMOR
		"]", //HELMET
		"(", //CLOAK
		"{", //GLOVES
		"\\",//BOOTS
		"=", //RING
		"\"",//AMULET
		"_", //LIGHT
		"~", //SCROLL
		"?", //BOOK
		"!", //FLASK
		"$", //GOLD
		"/", //AMMUNITION
		",", //FOOD
		"-", //WAND
		"%", //CONTAINER
		"*"
	};

	int symbol = types;
	int index = 0;
	while (!(symbol & 1) && index < static_cast<int>(SYMBOL.size()) - 1) {
		symbol >>= 1;
		index++;
	}

	return make_shared<Object>(
		dungeon,
		getRandomColor(colors),
		types,
		hit.roll(),
		dodge.roll(),
		def.roll(),
		weight.roll(),
		speed.roll(),
		attribute.roll(),
		value.roll(),
		dam,
		name,
		SYMBOL[index],
		SYMBOL_BORING[index],
		description
	);
}
