cmake_minimum_required(VERSION 3.10)
project(Adventure_Commander)

set(CMAKE_CXX_STANDARD 11)

if(WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /w14244")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

include_directories(include)
include_directories(library)

add_executable(Adventure_Commander
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
	source/driver.cpp
)

if(WIN32)
	target_link_libraries(Adventure_Commander 
		${CMAKE_CURRENT_LIST_DIR}/library/PDCurses/wingui/pdcurses.lib
	)
else()
	target_link_libraries(Adventure_Commander ncursesw)
endif()