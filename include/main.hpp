#ifndef MAIN_H
#define MAIN_H

#include <string>

#include "mob.hpp"
#include "entity.hpp"

using namespace std;

enum Error {
	FINE = 0,
	FILE_READ_BAD = 0x10,
	FILE_READ_BAD_HEAD,
	FILE_READ_BAD_VERSION,
	FILE_READ_GONE,
	FILE_READ_EOF = 0x20,
	FILE_READ_EOF_SIZE,
	FILE_READ_EOF_PLAYER,
	FILE_READ_EOF_HARDNESS,
	FILE_READ_EOF_ROOMS,
	ARGUMENT_BAD = 0x30,
	ARGUMENT_NO_DASH,
	ARGUMENT_UNKNOWN,
	ARGUMENT_REQ_PARAM,
	ARGUMENT_OOB,
};

int skewBetweenRange(int skew, int low, int high);
string& ltrim(string& str, const string& chars = "\t\r\n ");
string& rtrim(string& str, const string& chars = "\t\r\n ");
string& trim(string& str, const string& chars = "\t\r\n ");
Mob::Color getRandomColor(int colors);

#endif
