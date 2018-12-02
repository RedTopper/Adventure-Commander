#ifndef TILE_HPP
#define TILE_HPP

#include <string>

class Tile {
public:
	enum Type{
		VOID,
		EDGE,
		ROCK,
		ROOM,
		HALL,
	};

	enum Symbol {
		//Edges across board
		EDGE_N,
		EDGE_E,
		EDGE_S,
		EDGE_W,

		//Rocks
		ROCK_SOFT,
		ROCK_MED,
		ROCK_HARD,

		//Hall
		HALL_SYM,
		HALL_NESW,
		HALL_NES,
		HALL_ESW,
		HALL_SWN,
		HALL_WNE,
		HALL_NE,
		HALL_ES,
		HALL_SW,
		HALL_WN,
		HALL_NS,
		HALL_EW,

		//Rooms are air
		ROOM_SYM,
		VOID_SYM,
	};
	
	const static std::string getStr(Symbol sym) {
		switch(sym) {
			case EDGE_E:
			case EDGE_W:
				return u8"\u2588";
			case EDGE_N:
				return u8"\u2584";
			case EDGE_S:
				return u8"\u2580";
			case ROCK_SOFT:
				return u8"\u2591";
			case ROCK_MED:
				return u8"\u2592";
			case ROCK_HARD:
				return u8"\u2593";
			case HALL_SYM:
				return u8"\u2022";

			case HALL_NES:
				return u8"\u2560";
			case HALL_ESW:
				return u8"\u2566";
			case HALL_SWN:
				return u8"\u2563";
			case HALL_WNE:
				return u8"\u2569";
			case HALL_NE:
				return u8"\u255A";
			case HALL_ES:
				return u8"\u2554";
			case HALL_SW:
				return u8"\u2557";
			case HALL_WN:
				return u8"\u255D";
			case HALL_NS:
				return u8"\u2551";
			case HALL_EW:
				return u8"\u2550";
			case ROOM_SYM:
			case VOID_SYM:
			case HALL_NESW:
				return u8" ";
			default:
				return u8"X";
		}
	};

public:
	uint8_t hardness;
	Symbol symbol;
	Type type;
};

#endif
