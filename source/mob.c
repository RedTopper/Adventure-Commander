#include <wchar.h>
#include <stdlib.h>
#include <stdbool.h>

#include "mob.h"
#include "dungeon.h"
#include "queue.h"
#include "main.h"
#include "path.h"

#define SYM_PLAY_MACRO  L'@'

const int MAX_KNOWN_TURNS = 5;
const wchar_t SYM_PLAY = SYM_PLAY_MACRO;
const wchar_t MOB_TYPES[] = {
	L'0',
	L'1',
	L'2',
	L'3',
	L'4',
	L'5',
	L'6',
	L'7',
	L'8',
	L'9',
	L'a',
	L'b',
	L'c',
	L'd',
	L'e',
	L'f',
	SYM_PLAY_MACRO,
	L'?'
};

static wchar_t mobGetSymbol(Mob *mob) {
	int size = sizeof(MOB_TYPES)/sizeof(MOB_TYPES[0]);
	return MOB_TYPES[mob->skills >= size ? size : mob->skills];
}

static void mobPrintText(Dungeon* dungeon, Mob* mob, const wchar_t* text, const wchar_t* type) {
	size_t textLength = (size_t) (dungeon->dim.x) + 1;
	swprintf(dungeon->line2, textLength, L"%lc at (%d, %d) is %ls and %-*ls",
		mobGetSymbol(mob),
		mob->pos.x,
		mob->pos.y,
		type,
		textLength,
		text
	);
}

static Point mobValidSpawnPoint(Dungeon dungeon, Mob* mobs, int i) {
	Point pos = {0};

	while(1) {
		pos.x = rand() % dungeon.dim.x;
		pos.y = rand() % dungeon.dim.y;
		Tile tile = dungeon.tiles[pos.y][pos.x];

		//Spawn on floor
		if (!(tile.type == ROOM || tile.type == HALL)) continue;

		//Not on player
		if (pos.x == dungeon.player.pos.x || pos.y == dungeon.player.pos.y) continue;

		//Not on mob
		int onMob = 0;
		for (int mob = 0; !onMob && mob < i; mob++) {
			Mob m = mobs[mob];
			if (pos.x == m.pos.x && pos.y == m.pos.y) onMob = 1;
		}

		if (!onMob) break;
	}

	return pos;
}

static int mobCanSeePC(Dungeon* dungeon, Mob* mob) {
	Point current = mob->pos;
	Point end = dungeon->player.pos;
	Point sign;
	Point diff;
	diff.x = abs(end.x - current.x),
	diff.y = abs(end.y - current.y),
	sign.x = current.x < end.x ? 1 : -1;
	sign.y = current.y < end.y ? 1 : -1;

	int err = (diff.x > diff.y ? diff.x : -diff.y)/2, e2;

	while(!(current.x == end.x && current.y == end.y)) {
		Tile tile = dungeon->tiles[current.y][current.x];
		if (!(tile.type == HALL || tile.type == ROOM)) return 0;

		e2 = err;
		if (e2 > -diff.x) {
			err -= diff.y;
			current.x += sign.x;
		}

		if (e2 <  diff.y) {
			err += diff.x;
			current.y += sign.y;
		}
	}

	return 1;
}

static Point mobGetNextPoint(Point start, Point end) {
	if (start.x == end.x && start.y == end.y) return start;
	Point current = start;
	Point sign;
	Point diff;
	diff.x = abs(end.x - current.x),
	diff.y = abs(end.y - current.y),
	sign.x = current.x < end.x ? 1 : -1;
	sign.y = current.y < end.y ? 1 : -1;

	int err = (diff.x > diff.y ? diff.x : -diff.y)/2;
	if (err > -diff.x) {
		current.x += sign.x;
	}

	if (err <  diff.y) {
		current.y += sign.y;
	}

	return current;
}

static int mobMove(Dungeon* dungeon, Mob* mob, Point new) {
	Tile* tile = &dungeon->tiles[new.y][new.x];
	if (tile->type == ROCK && mob->skills & SKILL_TUNNELING) {
		if (tile->hardness <= HARDNESS_RATE) {
			tile->type = HALL;
			tile->hardness = 0;
			mob->pos = new;
			pathCreate(dungeon);
			return MOVE_BROKE_WALL;
		} else {
			tile->hardness -= HARDNESS_RATE;
			pathCreate(dungeon);
			return MOVE_DAMAGE_WALL;
		}
	} else if (tile->type == HALL || tile->type == ROOM){
		mob->pos = new;
		return MOVE_SUCCESS;
	}

	return MOVE_FAILURE;
}

static void mobTickStraightLine(Dungeon *dungeon, Mob *mob, const wchar_t* type) {
	Point next = mobGetNextPoint(mob->pos, dungeon->player.pos);
	Movement movement = mobMove(dungeon, mob, next);

	wchar_t* text;
	switch (movement) {
		case MOVE_BROKE_WALL:
			text = L"broke down the wall while beelining!";
			break;
		case MOVE_DAMAGE_WALL:
			text = L"damaged a wall while beelining!";
			break;
		case MOVE_SUCCESS:
			text = L"is beelining towards you!";
			break;
		default:
		case MOVE_FAILURE:
			text = L"tried to beeline, but failed!";
			break;
	}

	mobPrintText(dungeon, mob, text, type);
}

static void mobTickRandomly(Dungeon* dungeon, Mob* mob, const wchar_t* type) {
	int dir = rand() % (int)(sizeof(ADJACENT)/sizeof(ADJACENT[0]));
	Point new = {0};
	new.x = ADJACENT[dir].x + mob->pos.x;
	new.y = ADJACENT[dir].y + mob->pos.y;
	Movement movement = mobMove(dungeon, mob, new);
	wchar_t* text;
	switch (movement) {
		case MOVE_BROKE_WALL:
			text = L"broke down the wall!";
			break;
		case MOVE_DAMAGE_WALL:
			text = L"chipped at the wall!";
			break;
		case MOVE_SUCCESS:
			text = L"moved to a new spot!";
			break;
		default:
		case MOVE_FAILURE:
			text = L"did nothing!";
			break;
	}

	mobPrintText(dungeon, mob, text, type);
}

static void mobTickPathFind(Dungeon* dungeon, Mob* mob, const wchar_t* type) {
	int adj = sizeof(ADJACENT)/sizeof(ADJACENT[0]);
	Point next = {0};
	int lowest = INT32_MAX;
	for (int i = 0; i < adj; i++) {
		Point check;
		check = mob->pos;
		check.x += ADJACENT[i].x;
		check.y += ADJACENT[i].y;

		int dist;
		if (mob->skills & SKILL_TUNNELING) {
			dist = dungeon->pathDig[check.y][check.x];
		} else {
			dist = dungeon->pathFloor[check.y][check.x];
		}

		if (dist < lowest) {
			lowest = dist;
			next = check;
		}
	}

	if (lowest < INT32_MAX) {
		Movement movement;
		movement = mobMove(dungeon, mob, next);
		wchar_t* text;
		switch (movement) {
			case MOVE_BROKE_WALL:
				text = L"broke down the wall to get to you!";
				break;
			case MOVE_DAMAGE_WALL:
				text = L"damaged a wall to get to you!";
				break;
			case MOVE_SUCCESS:
				text = L"coming after you!";
				break;
			default:
			case MOVE_FAILURE:
				text = L"somehow failed to move?";
				break;
		}
		mobPrintText(dungeon, mob, text, type);
	} else {
		mobTickRandomly(dungeon, mob, L"trapped");
	}
}

//"Public" functions

void mobTick(Dungeon* dungeon, Mob* mob) {
	if (mob->hp <= 0) return;
	size_t textLength = (size_t)(dungeon->dim.x) + 1;
	setBufferPad(&dungeon->line2, L"Nothing important happened.", textLength);

	if (mob->skills & SKILL_PC) {
		//The player
		setBufferPad(&dungeon->line2, L"It's your turn but you lack effort to move...", textLength);
	} else if (mob->skills & SKILL_ERRATIC && rand() % 2) {
		//Not the player, but erratic movement
		mobTickRandomly(dungeon, mob, L"confused");
	} else if (mob->skills & SKILL_TELEPATHY) {
		//Not the player, not erratic, but has telepathy skill
		if (mob->skills & SKILL_INTELLIGENCE) {
			mobTickPathFind(dungeon, mob, L"telepathic");
		} else {
			mobTickStraightLine(dungeon, mob, L"telepathic");
		}
	} else if (mob->skills & SKILL_INTELLIGENCE) {
		//Not the player, not erratic, not telepathic, but has intelligence
		if (mobCanSeePC(dungeon, mob)) mob->known = 5;
		if (mob->known > 0) {
			mobTickPathFind(dungeon, mob, L"smart");
			mob->known--;
		} else {
			mobTickRandomly(dungeon, mob, L"searching");
		}
	} else {
		//Not the player, not erratic, not telepathic, not intelligent, but... idk, exists?
		if (mobCanSeePC(dungeon, mob)) {
			mobTickStraightLine(dungeon, mob, L"a bumbling idiot");
		} else {
			mobTickRandomly(dungeon, mob, L"a bumbling idiot");
		}
	}

	//Attack phase
	for (int i = 0; i < dungeon->numMobs + 1; i++) {
		//Make sure to include the player in the attack phase
		Mob* other = (i < dungeon->numMobs) ? &dungeon->mobs[i] : &dungeon->player;
		if (other == mob || other->hp == 0 || mob->pos.x != other->pos.x || mob->pos.y != other->pos.y) continue;
		other->hp--;
		wchar_t* text = (other->hp == 0) ? L"It killed it brutally!" : L"Looks like it hurt!";
		swprintf(dungeon->line2, textLength, L"%lc at (%d, %d) attacked %lc! %-*ls",
			mobGetSymbol(mob),
			mob->pos.x,
			mob->pos.y,
			mobGetSymbol(other),
			textLength,
			text
		);
	}
}

Mob mobGeneratePlayer(Point point) {
	Mob mob = {0};
	mob.pos = point;
	mob.known = 0;
	mob.skills = SKILL_PC;
	mob.speed = 10;
	mob.order = 1;
	mob.hp = 1;
	return mob;
}

Mob* mobGenerateAll(Dungeon dungeon) {
	Mob* mobs = malloc(sizeof(Mob) * dungeon.numMobs);
	for (int i = 0; i < dungeon.numMobs; i++) {
		Mob mob = {0};
		mob.pos = mobValidSpawnPoint(dungeon, mobs, i);
		mob.known = 0;
		mob.skills = rand() % 16;
		mob.speed = (rand() % 16) + 5; //5-20
		mob.order = i + 1;
		mob.hp = 1;
		mobs[i] = mob;
	}

	return mobs;
}

int mobAliveCount(Dungeon dungeon) {
	int count = 0;
	for (int i = 0; i < dungeon.numMobs; i++) {
		if (dungeon.mobs[i].hp > 0) count++;
	}
	return count;
}

Node* mobCreateQueue(Dungeon* dungeon) {
	Mob* player = &dungeon->player;
	Node* queue = queueCreateSub((NodeData){.mob=player}, 1000 / player->speed, player->order);
	for (int i = 0; i < dungeon->numMobs; i++) {
		Mob* m = &dungeon->mobs[i];
		queuePushSub(&queue, (NodeData){.mob=m}, 1000 / m->speed, m->order);
	}

	return queue;
}
