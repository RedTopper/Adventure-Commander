#define _XOPEN_SOURCE_EXTENDED

#include <ncursesw/curses.h>

#include "player.h"
#include "path.h"

static void playerMap(Mob *player, Dungeon *dungeon, WINDOW* base) {
	int offset = 0;
	int ch = 0;
	wclear(base);
	while (1) {
		//Clear screen and figure out characters
		int max = LINES - 2;
		werase(base);
		switch (ch) {
			case KEY_UP:
				if (offset == 0) beep(); //FALLTHROUGH
			case KEY_RESIZE:
				if (offset > 0) offset--;
				break;
			case KEY_DOWN:
				if (offset + max < dungeon->numMobs) {
					offset++;
				} else {
					beep();
				}
				break;
			case 27: //ESC
			case 'q':
				return;
			default:
				break;
		}

		//Show screen
		mvwaddwstr(base, 0, 0, L"Monster list. Press 'q' or 'ESC' to exit.");
		for (int line = 0; line < max; line++) {
			Mob* mob = line + offset < dungeon->numMobs ? &dungeon->mobs[line + offset] : NULL;
			mvwaddwstr(base, line + 1, 0, mob ? mobGetSymbol(mob, *dungeon) : L"~");
		}

		//Print end marker if needed
		if (offset + max >= dungeon->numMobs) {
			wattron(base, WA_STANDOUT);
			mvwaddwstr(base, LINES - 1, 0, L"(END)");
			wattroff(base, WA_STANDOUT);
		} else {
			mvwaddwstr(base, LINES - 1, 0, L":");
		}

		//Print and get new character
		wrefresh(base);
		ch = getch();
	}
}

static int playerIsOn(Mob *mob, Dungeon *dungeon, EntityType type) {
	for (int i = 0; i < dungeon->numEntities; i++) {
		Entity e = dungeon->entities[i];
		if (mob->pos.x == e.pos.x && mob->pos.y == e.pos.y && e.type == type) return 1;
	}

	return 0;
}

//"Public" functions

Action playerAction(Mob* mob, Dungeon* dungeon, WINDOW* base) {
	size_t len = (size_t) dungeon->dim.x;
	dungeonPrint(base, *dungeon);
	Action action = ACTION_NONE;
	Movement move = -1;
	int ch = getch();
	switch (ch) {
		case '7':
		case 'y':
			move = mobMove(mob, dungeon, pointAdd(mob->pos, (Point){-1,-1}));
			break;
		case '8':
		case 'k':
			move = mobMove(mob, dungeon, pointAdd(mob->pos, (Point){0,-1}));
			break;
		case '9':
		case 'u':
			move = mobMove(mob, dungeon, pointAdd(mob->pos, (Point){1,-1}));
			break;
		case '6':
		case 'l':
			move = mobMove(mob, dungeon, pointAdd(mob->pos, (Point){1,0}));
			break;
		case '3':
		case 'n':
			move = mobMove(mob, dungeon, pointAdd(mob->pos, (Point){1,1}));
			break;
		case '2':
		case 'j':
			move = mobMove(mob, dungeon, pointAdd(mob->pos, (Point){0,1}));
			break;
		case '1':
		case 'b':
			move = mobMove(mob, dungeon, pointAdd(mob->pos, (Point){-1,1}));
			break;
		case '4':
		case 'h':
			move = mobMove(mob, dungeon, pointAdd(mob->pos, (Point){-1,0}));
			break;
		case '5':
		case ' ':
			action = ACTION_STALL;
			break;
		case '>':
			if (playerIsOn(mob, dungeon, STAIRS_DOWN)) {
				action = ACTION_DOWN;
			} else {
				swprintf(dungeon->status, len,  L"You are not on a down staircase!");
			}
			break;
		case '<':
			if (playerIsOn(mob, dungeon, STAIRS_UP)) {
				action = ACTION_UP;
			} else {
				swprintf(dungeon->status, len,  L"You are not on an up staircase!");
			}
			break;
		case 'Q':
			action = ACTION_QUIT;
			break;
		case 'q':
			swprintf(dungeon->status, len, L"If you really want to quit, try 'shift-q' instead.");
			break;
		case 'm':
			playerMap(mob, dungeon, base);
			break;
		default:
			swprintf(dungeon->status, len, L"%d %ls", ch, L"is invalid!");
	}

	//Move the player character
	if (move == MOVE_FAILURE) swprintf(dungeon->status, len, L"I can't dig through that!");
	if (move == MOVE_SUCCESS) {
		action = ACTION_MOVE;
		pathCreate(dungeon);
	}

	return action;
}
