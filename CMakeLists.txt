cmake_minimum_required(VERSION 3.10)
project(Adventure_Commander)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)

add_executable(Adventure_Commander
	include/dungeon.hpp
	include/entity.hpp
	include/main.hpp
	include/mob.hpp
	include/path.hpp
	include/perlin.hpp
	include/player.hpp
	include/point.hpp
	include/tile.hpp
	include/room.hpp
	include/dice.hpp
	include/stream.hpp
	include/fmob.hpp
	include/fobject.hpp
	include/object.hpp
	include/factory.hpp
	source/dungeon.cpp
	source/entity.cpp
	source/main.cpp
	source/mob.cpp
	source/path.cpp
	source/perlin.cpp
	source/player.cpp
	source/dice.cpp
	source/stream.cpp
	source/fmob.cpp
	source/fobject.cpp
	source/object.cpp
	include/twist.hpp
)

target_link_libraries(Adventure_Commander ncursesw)
