#include "driver.hpp"

#ifdef _WIN32
	#define PDC_WIDE
	#include "PDCurses/curses.h"
	#include "windows.h"

	std::wstring utf8_decode(const std::string &str)
	{
		if (str.empty()) return std::wstring();
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
		std::wstring wstrTo(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
		return wstrTo;
	}

#elif __linux__
	#include <ncursesw/curses.h>
#else
	#error "driver.cpp found no sutable OS"
#endif

using namespace std;

Driver::Driver() {
	base = initscr();

	#ifdef _WIN32
	PDC_set_resize_limits(24, 100, 80, 200);
	#endif

	if (has_colors()) {
		use_default_colors();
		start_color();
		init_pair(Entity::RED, COLOR_RED, -1);
		init_pair(Entity::GREEN, COLOR_GREEN, -1);
		init_pair(Entity::BLUE, COLOR_BLUE, -1);
		init_pair(Entity::CYAN, COLOR_CYAN, -1);
		init_pair(Entity::YELLOW, COLOR_YELLOW, -1);
		init_pair(Entity::MAGENTA, COLOR_MAGENTA, -1);
		init_pair(Entity::WHITE, COLOR_WHITE, -1);
		init_pair(Entity::BLACK, 238, -1);
	}

	keypad(base, TRUE);
	cbreak();
	noecho();
	raw();
	wclear(base);
	curs_set(0);
}

Driver::~Driver() {
	end();
}

void Driver::wait() {
	ch();
}

void Driver::end() {
	if (running) endwin();
	running = false;
}

#ifdef _WIN32

//Windows is a baby and has to use wide characters
void Driver::str(const Point& pos, const std::string& str) {
	mvwaddwstr(base, pos.y, pos.x, utf8_decode(str).c_str());
}

#elif __linux__

//Manly linux can use utf8 by default like a champ
void Driver::str(const Point& pos, const std::string& str) {
	mvwaddstr(base, pos.y, pos.x, str.c_str());
}

#endif

void Driver::bold(const Point &pos, const std::string &str) {
	attron(WA_STANDOUT);
	Driver::str(pos, str);
	attroff(WA_STANDOUT);
}

void Driver::color(const Point& pos, Entity::Color color, const std::string& str) {
	attron(COLOR_PAIR(color));
	Driver::str(pos, str);
	attroff(COLOR_PAIR(color));
}

void Driver::clear() {
	werase(base);
}

void Driver::touch() {
	redrawwin(base);
}

void Driver::flip() {
	wrefresh(base);
}

int Driver::cols() {
	return COLS;
}

int Driver::rows() {
	return LINES;
}

int Driver::ch() {
	int ch = wgetch(base);

	switch(ch) {
		case KEY_UP:
			return DRV_UP;
		case KEY_DOWN:
			return DRV_DOWN;
		case KEY_LEFT:
			return DRV_LEFT;
		case KEY_RIGHT:
			return DRV_RIGHT;
		case KEY_END:
			return DRV_END;
		case KEY_HOME:
			return DRV_HOME;
		case KEY_NPAGE:
			return DRV_NPAGE;
		case KEY_PPAGE:
			return DRV_PPAGE;
		case KEY_B2:
			return DRV_B2;
		case KEY_ENTER:
			return DRV_ENTER;
		case KEY_RESIZE:
			resize_term(0, 0);
			return 0;
		default:
			break;
	}

	return ch;
}

void Driver::err() {
	beep();
}
