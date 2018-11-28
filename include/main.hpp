#ifndef MAIN_H
#define MAIN_H

#include <string>

#include "mob.hpp"

enum Error {
	FINE = 0,
	ENTROPY,
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
std::string& ltrim(std::string& str, const std::string& chars = "\t\r\n ");
std::string& rtrim(std::string& str, const std::string& chars = "\t\r\n ");
std::string& trim(std::string& str, const std::string& chars = "\t\r\n ");
std::vector<std::string> getOptions();
Mob::Color getRandomColor(int colors);

#endif
