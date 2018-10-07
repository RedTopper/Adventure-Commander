#include <wchar.h>
#include <stdlib.h>
#include <stdbool.h>

#include "mob.h"
#include "dungeon.h"
#include "queue.h"
#include "main.h"
#include "path.h"

#define SYM_PLAY_MACRO  L"@"

const int MAX_KNOWN_TURNS = 5;
const wchar_t* SYM_PLAY = SYM_PLAY_MACRO;
const wchar_t* MOB_TYPES[] = {
	L"\x01F480", //Skull   (    )
	L"\x01F464", //Empty   (I   )
	L"\x01F47B", //Ghost   ( T  )
	L"\x01F47F", //Demon   (IT  )
	L"\x01F417", //Boar    (  D )
	L"\x01F435", //Monkey  (I D )
	L"\x01F479", //Ogre    ( TD )
	L"\x01F432", //Dragon  (ITD )
	L"\x01F480", //Skull   (   E)
	L"\x01F419", //Squid   (I  E)
	L"\x01F40D", //Snake   ( D E)
	L"\x01F47A", //Goblin  (IT E)
	L"\x01F47E", //Monster (  DE)
	L"\x01F42F", //Tiger   (I DE)
	L"\x01F47D", //Alien   ( TDE)
	L"\x01F383", //Pumpkin (ITDE)
	SYM_PLAY_MACRO,
	L"?"
};

const wchar_t* MOB_TYPES_BORING[] = {
	L"sk", //Skull   (    )
	L"hu", //Empty   (I   )
	L"gh", //Ghost   ( T  )
	L"de", //Demon   (IT  )
	L"bo", //Boar    (  D )
	L"mo", //Monkey  (I D )
	L"OG", //Ogre    ( TD )
	L"DR", //Dragon  (ITD )
	L"sk", //Skull   (   E)
	L"sq", //Squid   (I  E)
	L"sn", //Snake   ( D E)
	L"go", //Goblin  (IT E)
	L"MO", //Monster (  DE)
	L"TI", //Tiger   (I DE)
	L"AL", //Alien   ( TDE)
	L"PU", //Pumpkin (ITDE)
	SYM_PLAY_MACRO,
	L"?"
};

static void mobPrintText(Mob* mob, Dungeon* dungeon, const wchar_t* text, const wchar_t* type) {
	size_t textLength = (size_t) (dungeon->dim.x) + 1;
	swprintf(dungeon->status, textLength, L"%ls at (%d, %d) is %ls and %-*ls",
		mobGetSymbol(mob, *dungeon),
		mob->pos.x,
		mob->pos.y,
		type,
		textLength,
		text
	);
}

static Point mobValidSpawnPoint(Mob* mobs, Dungeon dungeon, int i) {
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

static int mobCanSeePC(Mob* mob, Dungeon* dungeon) {
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

static int mobMove(Mob* mob, Dungeon* dungeon, Point new) {
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

static void mobTickStraightLine(Mob* mob, Dungeon* dungeon, const wchar_t* type) {
	Point next = mobGetNextPoint(mob->pos, dungeon->player.pos);
	Movement movement = mobMove(mob, dungeon, next);

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

	mobPrintText(mob, dungeon, text, type);
}

static void mobTickRandomly(Mob* mob, Dungeon* dungeon, const wchar_t* type) {
	int dir = rand() % (int)(sizeof(ADJACENT)/sizeof(ADJACENT[0]));
	Point new = {0};
	new.x = ADJACENT[dir].x + mob->pos.x;
	new.y = ADJACENT[dir].y + mob->pos.y;
	Movement movement = mobMove(mob, dungeon, new);
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

	mobPrintText(mob, dungeon, text, type);
}

static void mobTickPathFind(Mob* mob, Dungeon* dungeon, const wchar_t* type) {
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
		movement = mobMove(mob, dungeon, next);
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
		mobPrintText(mob, dungeon, text, type);
	} else {
		mobTickRandomly(mob, dungeon, L"trapped");
	}
}

//"Public" functions

const wchar_t* mobGetSymbol(Mob* mob, Dungeon dungeon) {
	int size = sizeof(MOB_TYPES)/sizeof(MOB_TYPES[0]);
	if (dungeon.emoji) {
		return MOB_TYPES[mob->skills >= size ? size : mob->skills];
	} else {
		return MOB_TYPES_BORING[mob->skills >= size ? size : mob->skills];
	}
}

void mobTick(Mob* mob, Dungeon* dungeon) {
	if (mob->hp <= 0) return;
	size_t textLength = (size_t)(dungeon->dim.x) + 1;
	setText(*dungeon, &dungeon->status, L"Nothing important happened.");

	if (mob->skills & SKILL_PC) {
		//The player
		setText(*dungeon, &dungeon->status, L"It's your turn!");
	} else if (mob->skills & SKILL_ERRATIC && rand() % 2) {
		//Not the player, but erratic movement
		mobTickRandomly(mob, dungeon, L"confused");
	} else if (mob->skills & SKILL_TELEPATHY) {
		//Not the player, not erratic, but has telepathy skill
		if (mob->skills & SKILL_INTELLIGENCE) {
			mobTickPathFind(mob, dungeon, L"telepathic");
		} else {
			mobTickStraightLine(mob, dungeon, L"telepathic");
		}
	} else if (mob->skills & SKILL_INTELLIGENCE) {
		//Not the player, not erratic, not telepathic, but has intelligence
		if (mobCanSeePC(mob, dungeon)) mob->known = MAX_KNOWN_TURNS;
		if (mob->known > 0) {
			mobTickPathFind(mob, dungeon, L"smart");
			mob->known--;
		} else {
			mobTickRandomly(mob, dungeon, L"searching");
		}
	} else {
		//Not the player, not erratic, not telepathic, not intelligent, but... idk, exists?
		if (mobCanSeePC(mob, dungeon)) {
			mobTickStraightLine(mob, dungeon, L"a bumbling idiot");
		} else {
			mobTickRandomly(mob, dungeon, L"a bumbling idiot");
		}
	}

	//Attack phase
	for (int i = 0; i < dungeon->numMobs + 1; i++) {
		//Make sure to include the player in the attack phase
		Mob* other = (i < dungeon->numMobs) ? &dungeon->mobs[i] : &dungeon->player;

		//Collision detection. Monsters are wide so overlapping includes right one.
		if (other == mob
			|| other->hp == 0
			|| (other->pos.x != mob->pos.x && other->pos.x - 1 != mob->pos.x)
			|| mob->pos.y != other->pos.y) continue;

		other->hp--;
		wchar_t* text = (other->hp == 0) ? L"It killed it brutally!" : L"Looks like it hurt!";
		swprintf(dungeon->status, textLength, L"%ls at (%d, %d) attacked %ls! %-*ls",
			mobGetSymbol(mob, *dungeon),
			mob->pos.x,
			mob->pos.y,
			mobGetSymbol(other, *dungeon),
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
	mob.order = 0;
	mob.hp = 1;
	return mob;
}

Mob* mobGenerateAll(Dungeon dungeon) {
	Mob* mobs = malloc(sizeof(Mob) * dungeon.numMobs);
	for (int i = 0; i < dungeon.numMobs; i++) {
		Mob mob = {0};
		mob.pos = mobValidSpawnPoint(mobs, dungeon, i);
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
