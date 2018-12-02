#ifndef ADVENTURE_COMMANDER_DRIVER_HPP
#define ADVENTURE_COMMANDER_DRIVER_HPP

#include <string>

#include "point.hpp"
#include "entity.hpp"

#ifdef _WIN32
	struct _win;
	typedef _win WINDOW;
#elif __linux__
	struct _win_st;
	typedef _win_st WINDOW;
#else
	#error "driver.cpp found no sutable conversion to WINDOW"
#endif

class Driver {
public:
	enum Key {
		DRV_UP = 0x100000,
		DRV_DOWN,
		DRV_LEFT,
		DRV_RIGHT,
		DRV_END,
		DRV_HOME,
		DRV_PPAGE,
		DRV_NPAGE,
		DRV_B2,
		DRV_ENTER,
	};

private:
	WINDOW* base;
	bool running = true;

public:
	Driver();
	~Driver();

	void str(const Point& pos, const std::string& str);
	void bold(const Point& pos, const std::string& str);
	void color(const Point& pos, Entity::Color color, const std::string& str);
	void wait();
	void end();
	void clear();
	void touch();
	void flip();
	void err();
	int cols();
	int rows();
	int ch();
};

#endif
