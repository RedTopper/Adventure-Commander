#include <wchar.h>
#include <stdlib.h>

#include "mob.h"
#include "dungeon.h"
#include "queue.h"
#include "main.h"
#include "path.h"

#define SYM_PLAY_MACRO  L'@'

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
		int exists = 0;
		for (int mob = 0; !exists && mob < i; mob++) {
			Mob m = mobs[mob];
			if (pos.x == m.pos.x && pos.y == m.pos.y) exists = 1;
		}

		if (!exists) break;
	}

	return pos;
}

static void mobTickRandomly(Dungeon* dungeon, Mob* mob, const wchar_t* textFlavor) {
	wchar_t* text = L"did nothing!";
	int dir = rand() % (int)(sizeof(ADJACENT)/sizeof(ADJACENT[0]));
	Point new = {0};
	new.x = ADJACENT[dir].x + mob->pos.x;
	new.y = ADJACENT[dir].y + mob->pos.y;
	Tile* tile = &dungeon->tiles[new.y][new.x];
	if (tile->type == ROCK && mob->skills & TUNNELING) {
		if (tile->hardness <= HARDNESS_RATE) {
			tile->type = HALL;
			tile->hardness = 0;
			text = L"broke down the wall!";
			mob->pos = new;
		} else {
			tile->hardness -= HARDNESS_RATE;
			text = L"chipped at the wall!";
		}

		pathCreate(dungeon);
	} else if (tile->type == HALL || tile->type == ROOM){
		mob->pos = new;
		text = L"moved to a new spot!";
	}

	size_t textLength = (size_t) (dungeon->dim.x) + 1;
	swprintf(dungeon->line2, textLength, L"%lc at (%d, %d) %ls and %-*ls",
		mobGetSymbol(mob),
		mob->pos.x,
		mob->pos.y,
		textFlavor,
		textLength,
		text
	);
}

//"Public" functions

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

Mob mobGeneratePlayer(Point point) {
	Mob mob = {0};
	mob.pos = point;
	mob.known = (Point){0};
	mob.skills = PC;
	mob.speed = 10;
	mob.order = 1;
	mob.hp = 1;
	return mob;
}

void mobTick(Dungeon* dungeon, Mob* mob) {
	if (mob->hp <= 0) return;
	size_t textLength = (size_t)(dungeon->dim.x) + 1;
	setBufferPad(&dungeon->line2, L"Nothing important happened.", textLength);

	if (mob->skills & PC) {
		//The player
		setBufferPad(&dungeon->line2, L"It's your turn but you lack effort to move...", textLength);
	} else if (mob->skills & ERRATIC && rand() % 2) {
		//Not the player, but erratic movement
		mobTickRandomly(dungeon, mob, L"got confused");
	} else if (mob->skills & TELEPATHY) {
		//Not the player, not erratic, but has telepathy skill
	} else if (mob->skills & INTELLIGENCE) {
		//Not the player, not erratic, not telepathic, but has intelligence
	} else {
		//Not the player, not erratic, not telepathic, not intelligent, but... idk, exists?
		mobTickRandomly(dungeon, mob, L"is a bumbling idiot");
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

Mob* mobGenerateAll(Dungeon dungeon) {
	Mob* mobs = malloc(sizeof(Mob) * dungeon.numMobs);
	for (int i = 0; i < dungeon.numMobs; i++) {
		Mob mob = {0};
		mob.pos = mobValidSpawnPoint(dungeon, mobs, i);
		mob.known = (Point){0};
		mob.skills = rand() % 16;
		mob.speed = (rand() % 16) + 5; //5-20
		mob.order = i + 1;
		mob.hp = 1;
		mobs[i] = mob;
	}

	return mobs;
}
