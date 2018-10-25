#ifndef TILE_HPP
#define TILE_HPP

#include <cstdint>

class Tile {
public:
	enum Type{
		VOID,
		EDGE,
		ROCK,
		ROOM,
		HALL,
	};

	enum Symbol : wchar_t {
		//Edges across board
		EDGE_N = L'\x2584',
		EDGE_E = L'\x2588',
		EDGE_S = L'\x2580',
		EDGE_W = L'\x2588',

		//Rocks
		ROCK_SOFT = L'\x2591',
		ROCK_MED = L'\x2592',
		ROCK_HARD = L'\x2593',

		//Hall
		HALL_SYM = L'\x2022',
		HALL_NESW = L' ',
		HALL_NES = L'\x2560',
		HALL_ESW = L'\x2566',
		HALL_SWN = L'\x2563',
		HALL_WNE = L'\x2569',
		HALL_NE = L'\x255A',
		HALL_ES = L'\x2554',
		HALL_SW = L'\x2557',
		HALL_WN = L'\x255D',
		HALL_NS = L'\x2551',
		HALL_EW = L'\x2550',

		//Rooms are air
		ROOM_SYM = L' ',
		VOID_SYM = L' ',
	};

public:
	uint8_t hardness;
	Symbol symbol;
	Type type;
};

#endif
