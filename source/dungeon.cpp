#include <algorithm>
#include <fstream>
#include <limits>

#include "twist.hpp"
#include "dungeon.hpp"
#include "perlin.hpp"
#include "main.hpp"
#include "player.hpp"
#include "fmob.hpp"
#include "fobject.hpp"
#include "driver.hpp"

using namespace std;

//default size
const Point DUNGEON_DIM = Point(80, 21); // NOLINT(cert-err58-cpp)

//Number of rooms
static const int ROOMS_MIN = 5;
static const int ROOMS_MAX = 8;
static const float ROOM_MAX_FULLNESS = 0.15f;

//Size of rooms. 
static const int ROOM_X_MIN = 7;
static const int ROOM_X_MAX = 16;
static const int ROOM_Y_MIN = 3;
static const int ROOM_Y_MAX = 8;

//Randomness of midpoint, radius
static const int ROOM_CON_RAD = 4;

//Magic header
static const string HEADER = "RLG327-F2018"; // NOLINT(cert-err58-cpp)
static const uint32_t VERSION = 0;

//Should return false if lhs is considered to go before rhs
//Used to declare the turn order of mobs/player during the game loop
bool Order::operator()(const shared_ptr<Mob> &lhs, const shared_ptr<Mob> &rhs) const {
	return !lhs->isBefore(*rhs);
}

void Dungeon::connectRoomRasterize(const Point &from, const Point &to) {
	Point current = from;
	Point dist = Point();
	Point step = Point();
	
	dist.x = abs(to.x - from.x);
	dist.y = -abs(to.y - from.y);
	step.x = (from.x < to.x ? 1 : -1);
	step.y = (from.y < to.y ? 1 : -1);
	
	int error = dist.x + dist.y;

	placeHall(current);
	
	while (current.x != to.x || current.y != to.y) {
		if (2 * error - dist.y > dist.x - 2 * error) {
			// horizontal step
			error += dist.y;
			current.x += step.x;
		} else {
			// vertical step
			error += dist.x;
			current.y += step.y;
		}

		placeHall(current);
	}
}

void Dungeon::connectRoom(const Room &first, const Room &second) {
	Point start = Twist::rand(first.dim - 1) + first.pos;
	Point end = Twist::rand(second.dim - 1) + second.pos;
	Point offset = Twist::rand(Point(-ROOM_CON_RAD), Point(ROOM_CON_RAD));
	Point mid = Point();

	//Get the actual midpoint.
	mid.x = (int)(((float)(start.x) + (float)(end.x)) / 2.0f + (float)offset.x);
	mid.y = (int)(((float)(start.y) + (float)(end.y)) / 2.0f + (float)offset.y);

	//Check bounds of midpoint.
	if(mid.x < 1) mid.x = 1;
	if(mid.y < 1) mid.y = 1;
	if(mid.x > dim.x - 2) mid.x = dim.x - 2;
	if(mid.y > dim.y - 2) mid.y = dim.y - 2;

	connectRoomRasterize(start, mid);
	connectRoomRasterize(mid, end);
}

bool Dungeon::placeRoomAttempt(const Room &room) {
	for (int row = room.pos.y - 1; row < room.pos.y + room.dim.y + 1; row++) {
		for (int col = room.pos.x - 1; col < room.pos.x + room.dim.x + 1; col++) {
			if (row < 0) return false;
			if (col < 0) return false;
			if (row >= dim.y) return false;
			if (col >= dim.x) return false;
			if (tiles[row][col].type == Tile::ROOM) return false;
		}
	}

	return true;
}

void Dungeon::placeRoom(const Room &room) {
	for (int row = room.pos.y; row < room.pos.y + room.dim.y; row++) {
		for (int col = room.pos.x; col < room.pos.x + room.dim.x; col++) {
			Tile& tile = tiles[row][col];
			tile.type = Tile::ROOM;
			tile.symbol = Tile::ROOM_SYM;
			tile.hardness = 0;
		}
	}

	rooms.push_back(room);
}

void Dungeon::placeHall(const Point &point) {
	if (point > Point() && point < (dim - Point(1,1))) {
		Tile& tile = tiles[point.y][point.x];
		if (tile.type == Tile::ROCK) {
			tile.type = Tile::HALL;
			tile.symbol = Tile::HALL_SYM;
			tile.hardness = 0;
		}
	}
}

void Dungeon::placeTile(const Point &pos, uint8_t hardness, const float *seed) {
	Tile tile;
	
	//Defaults
	tile.symbol = Tile::VOID_SYM;
	tile.type = Tile::VOID;
	tile.hardness = 0xFF;

	if (seed != nullptr) {
		//We were given a seed to generate the map
		if (pos.y == 0 || pos.y == dim.y - 1 || pos.x == 0 || pos.x == dim.x - 1) {
			tile.type = Tile::EDGE;
			tile.hardness = 0xFF;
		} else {
			tile.type = Tile::ROCK;
			tile.hardness = (uint8_t)(noise2D(dim, pos, seed, 4, 0.2f) * 255.0f);
		}
	} else if (hardness == 0xFF) {
		//Edge if hardness is 255.
		tile.type = Tile::EDGE;
		tile.hardness = 0xFF;
	} else if (hardness == 0x00) {
		//Hallway if hardness is 0. Will convert rooms to rooms during load.
		tile.type = Tile::HALL;
		tile.hardness = 0x00;
	} else {
		//If not max hardness or not min hardness, then it's a rock.
		tile.type = Tile::ROCK;
		tile.hardness = hardness;
	}

	tiles[pos.y][pos.x] = tile;
}

void Dungeon::generateRooms() {
	Room room;

	do {
		room.dim.x = Twist::geom(0.25, ROOM_X_MIN, ROOM_X_MAX);
		room.dim.y = Twist::geom(0.25, ROOM_Y_MIN, ROOM_Y_MAX);
		room.pos = Twist::rand(Point(1), dim - room.dim - 1);
	} while (!placeRoomAttempt(room));

	placeRoom(room);
}

template <class F, class T>
vector<shared_ptr<T>> Dungeon::generateFactory(vector<F>& factories, int total) {
	vector<shared_ptr<T>> out;
	int generated = 0;
	bool running = true;
	while(running) {
		for (Factory& factory : factories) {
			if (generated == total) {
				running = false;
				break;
			}

			int chance = Twist::rand(100);
			if (!(factory.isCreatable() && !factory.isSpawned() && chance <= factory.getRarity())) continue;
			out.push_back(static_pointer_cast<T>(factory.make(this, generated + 1)));
			factory.setSpawned(true);
			generated++;
		}
	}

	return out;
}

void Dungeon::generateEntities(int floor) {
	//Create up stairs always
	shared_ptr<Entity> up = make_shared<Entity>(this, Entity::STAIRS_UP, Entity::Color::WHITE, false);
	entities.push_back(up);

	if (floor) {
		//Above the 0th floor, allow player to go down.
		shared_ptr<Entity> down = make_shared<Entity>(this, Entity::STAIRS_DOWN, Entity::Color::WHITE, false);
		down->setPos(player->getPos());
		entities.push_back(down);
	}
}

void Dungeon::postProcess(vector<vector<Tile>>& tiles) {
	for(int row = 0; row < dim.y; row++) {
		for(int col = 0; col < dim.x; col++) {
			Tile& tile = tiles[row][col];

			//Render rock hardness
			if (tile.type == Tile::ROCK) {
				if (tile.hardness < 0x55) {
					tile.symbol = Tile::ROCK_SOFT;
				} else if (tile.hardness < 0xAA) {
					tile.symbol = Tile::ROCK_MED;
				} else {
					tile.symbol = Tile::ROCK_HARD;
				}
			}

			//Render the edge
			if (tile.type == Tile::EDGE) {
				if (row == 0) {
					tile.symbol = Tile::EDGE_N;
				} else if (row == dim.y - 1) {
					tile.symbol = Tile::EDGE_S;
				} else if (col == 0) {
					tile.symbol = Tile::EDGE_W;
				} else if (col == dim.x - 1) {
					tile.symbol = Tile::EDGE_E;
				}
			}

			if (tile.type == Tile::ROOM) {
				tile.symbol = Tile::ROOM_SYM;
			}

			if (tile.type != Tile::HALL) continue;

			Tile& tileN = tiles[row - 1][col];
			Tile& tileE = tiles[row][col + 1];
			Tile& tileS = tiles[row + 1][col];
			Tile& tileW = tiles[row][col - 1];
			
			int NESW = 0x000F
				& (((tileN.type == Tile::HALL || tileN.type == Tile::ROOM) << 3)
				| ((tileE.type == Tile::HALL || tileE.type == Tile::ROOM) << 2)
				| ((tileS.type == Tile::HALL || tileS.type == Tile::ROOM) << 1)
				| (tileW.type == Tile::HALL || tileW.type == Tile::ROOM));
			
			switch (NESW) {
				case 0x0F: //1111
					tile.symbol = Tile::HALL_NESW;
					break;
				case 0x0E: //1110
					tile.symbol = Tile::HALL_NES;
					break;
				case 0x07: //0111
					tile.symbol = Tile::HALL_ESW;
					break;
				case 0x0B: //1011
					tile.symbol = Tile::HALL_SWN;
					break;
				case 0x0D: //1101
					tile.symbol = Tile::HALL_WNE;
					break;
				case 0x0C: //1100
					tile.symbol = Tile::HALL_NE;
					break;
				case 0x06: //0110
					tile.symbol = Tile::HALL_ES;
					break;
				case 0x03: //0011
					tile.symbol = Tile::HALL_SW;
					break;
				case 0x09: //1001
					tile.symbol = Tile::HALL_WN;
					break;
				case 0x0A: //1010
					tile.symbol = Tile::HALL_NS;
					break;
				case 0x05: //0101
					tile.symbol = Tile::HALL_EW;
					break;
				case 0x08: //1000
				case 0x02: //0010
					tile.symbol = Tile::HALL_NS;
					break;
				case 0x04: //0100
				case 0x01: //0001
					tile.symbol = Tile::HALL_EW;
					break;
				default:  //No chars for stubs.
					tile.symbol = Tile::HALL_SYM;
			}
		}
	}
}

void Dungeon::finalize(shared_ptr<Driver>& base, vector<FMob>& fMob, vector<FObject>& fObject, shared_ptr<Player>& player, int floor, bool emoji, int count) {

	//Generate a new player (if needed)
	if (player == nullptr) {
		player = make_shared<Player>(this, base);
	} else {
		player->setDungeon(this);
	}

	//Shuffles the order of the factories so they produce interesting dungeons.
	shuffle(fMob.begin(), fMob.end(), Twist::getTwister());
	shuffle(fObject.begin(), fObject.end(), Twist::getTwister());

	this->emoji = emoji;
	this->player = player;
	this->player->setTurn(0);

	if (playerPos.isZero()) {
		//Place the player in the middle of the first room
		this->player->move(Point(rooms[0].pos.x + (rooms[0].dim.x)/2, rooms[0].pos.y + (rooms[0].dim.y)/2));
	} else {
		//Player is returning to this dungeon
		this->player->move(playerPos);
	}

	//Reset factories
	for(auto& m : fMob) m.setSpawned(false);

	//Create mobs
	mobs = generateFactory<FMob, Mob>(fMob, count + floor > 100 ? 100 : count + floor);
	objects = generateFactory<FObject, Object>(fObject, 10);
	generateEntities(floor);
	recalculate();

	//Create the turn queue
	turn = Turn();
	turn.push(player);
	for (const auto& m : mobs) {
		turn.push(m);
	}

	//Some default text
	status = "Nothing has happened yet!";
	line1 = "Adventure Commander";
	line2 = "Status Text";

	//slap in some fog
	display = FOGGY;
	for (auto& row : fog) {
		for (auto& tile : row) {
			tile.type = Tile::VOID;
			tile.symbol = Tile::VOID_SYM;
			tile.hardness = 0;
		}
	}
}

Dungeon::Dungeon(const Point& dim)
		: map(this, Path::VIA_FLOOR), dig(this, Path::VIA_DIG) {
	if (dim < Point(1,1)) return;
	this->dim = dim;
	
	//Get seed for noise
	vector<float> seed;
	seed.resize((unsigned  long)(dim.y) * (unsigned  long)(dim.x));
	for (int i = 0; i < dim.y * dim.x; i++) {
		seed[i] = (float)Twist::rand();
	}

	//Initialize dungeon
	tiles = vector<vector<Tile>>((unsigned long)(dim.y), vector<Tile>((unsigned long)(dim.x)));
	fog = tiles;
	for(int row = 0; row < dim.y; row++) {
		for(int col = 0; col < dim.x; col++) {
			placeTile(Point(col, row), 0x01, seed.data());
		}
	}
	
	//Create rooms
	for(int count = 0; count < ROOMS_MAX; count++) {
		if (count >= ROOMS_MIN && isFull()) break;
		generateRooms();
	}
	
	//Sort (for consistency)
	sort(rooms.begin(), rooms.end(), [dim](const Room& lhs, const Room& rhs) {
		const Point lhsCenter = Point(lhs.pos) += Point(lhs.dim.x/2, lhs.dim.y/2);
		const Point rhsCenter = Point(rhs.pos) += Point(rhs.dim.x/2, rhs.dim.y/2);
		return lhsCenter.less(Point(dim.x/2, dim.y/2), rhsCenter);
	});

	//Create the paths.
	connectRoom(rooms[0], rooms[rooms.size() - 1]);
	for(uint32_t i = 0; i < rooms.size() - 1; i++) {
		connectRoom(rooms[i], rooms[i + 1]);
	}

	//No need to restore position later
	playerPos = Point();
}

Dungeon::Dungeon(fstream& file)
		: map(this, Path::VIA_FLOOR), dig(this, Path::VIA_DIG) {
	size_t length = HEADER.length();
	vector<char> head;
	head.resize(length + 1);
	this->dim = DUNGEON_DIM;
	
	//Read magic header
	file.read(head.data(), length);
	head[length] = '\0';
	if (!file || HEADER != head.data()) {
		cout << "Bad file header '" << head.data() << "'!" << endl;
		exit(FILE_READ_BAD_HEAD);
	}
	
	//Check file version
	uint32_t version;
	file.read(reinterpret_cast<char*>(&version), sizeof(version));
	version = swap(version);
	if (!file || version != VERSION) {
		cout << "Bad file version!" << endl;
		exit(FILE_READ_BAD_VERSION);
	}
	
	//Get file size
	uint32_t size;
	file.read(reinterpret_cast<char*>(&size), sizeof(size));
	size = swap(size);
	if (!file) {
		cout << "Bad file size (EOF)!" << endl;
		exit(FILE_READ_EOF_SIZE);
	}
	
	//Get position
	uint8_t pos[2];
	file.read(reinterpret_cast<char*>(&pos), sizeof(pos));
	if (!file || pos[0] >= dim.x || pos[1] >= dim.y) {
		cout << "Bad file player co-ordinates (EOF)!" << endl;
		exit(FILE_READ_EOF_PLAYER);
	}
	
	//Initialize dungeon
	tiles = vector<vector<Tile>>((unsigned long)(dim.y), vector<Tile>((unsigned long)(dim.x)));
	fog = tiles;
	for(int row = 0; row < dim.y; row++) {
		for(int col = 0; col < dim.x; col++) {
			uint8_t hardness;
			file.read(reinterpret_cast<char*>(&hardness), sizeof(hardness));
			
			if (!file) {
				cout << "Missing tile harness information (EOF)!" << endl;
				exit(FILE_READ_EOF_HARDNESS);
			}

			placeTile(Point(col, row), hardness, nullptr);
		}
	}
	
	//Get rooms
	while (file.tellg() < size) {
		Room room;
		uint8_t data[4];
		file.read(reinterpret_cast<char*>(&data), sizeof(data));
		if (!file) {
			cout << "Missing rooms information (EOF)!" << endl;
			exit(FILE_READ_EOF_ROOMS);
		}

		uint8_t posX = data[0];
		uint8_t posY = data[1];
		uint8_t dimX = data[2];
		uint8_t dimY = data[3];
		
		//Verify the rooms are in bounds. If they are not, throw them out.
		room.pos.x = posX >= dim.x ? dim.x - 1 : posX;
		room.pos.y = posY >= dim.x ? dim.x - 1 : posY;
		room.dim.x = dimX + room.pos.x >= dim.x ? 1 : dimX;
		room.dim.y = dimY + room.pos.y >= dim.x ? 1 : dimY;

		placeRoom(room);
	}

	//Need to be able to restore loaded position
	playerPos = Point(pos[0], pos[1]);
}

void Dungeon::save(fstream& file) {
	//Header
	file.write(HEADER.c_str(), HEADER.length());
	
	//Version
	uint32_t ver = swap(VERSION);
	file.write(reinterpret_cast<char*>(&ver), sizeof(ver));
	
	//Size of file, 22 for the header, width and height of dungeon, and extra rooms
	uint32_t size = swap((uint32_t)(22 + dim.x * dim.y + rooms.size() * 4));
	file.write(reinterpret_cast<char*>(&size), sizeof(size));
	
	//Player position
	uint8_t player[2] = {(uint8_t)this->player->getPos().x, (uint8_t)this->player->getPos().y};
	file.write(reinterpret_cast<char*>(&player), sizeof(player));
	
	//Tiles
	for(int row = 0; row < dim.y; row++) {
		for(int col = 0; col < dim.x; col++) {
			uint8_t hardness = tiles[row][col].hardness;
			file.write(reinterpret_cast<char*>(&hardness), sizeof(hardness));
		}
	}
	
	//Rooms
	for (auto room : rooms) {
		uint8_t data[4] = {
			(uint8_t) (room.pos.x),
			(uint8_t) (room.pos.y),
			(uint8_t) (room.dim.x),
			(uint8_t) (room.dim.y),
		};

		file.write(reinterpret_cast<char*>(&data), sizeof(data));
	}
}

bool Dungeon::alive() const {
	for (const auto& m : mobs) {
		if ((m->getSkills() & Mob::SK_BOSS) && !m->isAlive()) return false;
	}

	return true;
}

void Dungeon:: rotate() {
	shared_ptr<Mob> mob = turn.top();
	mob->nextTurn();
	turn.pop();
	turn.push(mob);
}

void Dungeon::updateFoggy() {
	if (!player) return;
	for (int row = player->getPos().y - FOG_Y; row <= player->getPos().y + FOG_Y; row++) {
		for (int col = player->getPos().x - FOG_X; col <= player->getPos().x + FOG_X; col++) {
			if (row >= 0 && col >= 0 && row < dim.y && col < dim.x) {
				fog[row][col] = tiles[row][col];
			}
		}
	}
}

int Dungeon::isFull() {
	auto total = (float)(dim.x * dim.y);
	float room = 0.0;
	for(int row = 0; row < dim.y; row++) {
		for(int col = 0; col < dim.x; col++) {
			if (tiles[row][col].type == Tile::ROOM) room = room + 1.0f;
		}
	}

	return room/total > ROOM_MAX_FULLNESS;
}

bool Dungeon::isInRange(const Entity &e) {
	return display != FOGGY || e.isRemembered() || (player
		&& e.getPos().x - player->getPos().x >= -FOG_X
		&& e.getPos().x - player->getPos().x <= FOG_X
		&& e.getPos().y - player->getPos().y >= -FOG_Y
		&& e.getPos().y - player->getPos().y <= FOG_Y);
}

template <class T>
bool Dungeon::isLeft(const shared_ptr<Entity> &original, vector<shared_ptr<T>> list) {
	for (const auto& obj : list) {
		//For type hinting information
		const shared_ptr<Entity>& other = obj;

		//Skip dead monsters
		auto mob = dynamic_pointer_cast<Mob>(other);
		if (mob != nullptr && !mob->isAlive()) continue;

		//Check against the list
		if (original->getPos().x == other->getPos().x - 1 && original->getPos().y == other->getPos().y) {
			return true;
		}
	}

	return false;
}


void Dungeon::printEntity(shared_ptr<Driver>& base, const shared_ptr<Entity> &e) {
	//check if the mob is directly to the left of another mob.
	//An emoji takes two characters on some terminals, so they can't overlap

	bool left = false;
	if (isLeft<Mob>(e, mobs) || isLeft<Object>(e, objects) || isLeft<Entity>(e, entities)) left = true;

	//Also check the player
	if (!left) {
		left = e->getPos().x == player->getPos().x - 1 && e->getPos().y == player->getPos().y;
	}

	//Render only if the player is near
	base->color(e->getPos() + Point(0, 1), e->getColor(), left ? e->getSymbolAlt() : e->getSymbol());
}

void Dungeon::print(shared_ptr<Driver>& base) {
	//clean window
	base->clear();

	//Render fog or tiles
	vector<vector<Tile>> &tiles = display == FOGGY ? this->fog : this->tiles;

	//Make the dungeon look nice
	postProcess(tiles);

	//Write tiles
	for (int row = 0; row < dim.y; row++) {
		stringstream line;
		for (int col = 0; col < dim.x; col++) {
			int dist = 0;
			if (display == DISTANCE) {
				dist = map.getDist(Point(col, row));
			} else if (display == TUNNEL) {
				dist = dig.getDist(Point(col, row)) % 10;
			} else {
				line << Tile::getStr(tiles[row][col].symbol);
				continue;
			}

			if (dist == INT32_MAX) {
				line << " ";
			} else {
				line << dist % 10;
			}
		}

		base->str(Point(0, row + 1), line.str());
	}

	//Output objects
	for (auto& o : objects) {
		if (!isInRange(*o)) continue;
		o->setRemembered(true);
		printEntity(base, o);
	}

	//Write entities
	for (auto& e : entities) {
		if (!isInRange(*e)) continue;
		e->setRemembered(true);
		printEntity(base, e);
	}

	//Write players
	if(player) {
		printEntity(base, player);
	}

	//Write mobs
	for (const auto& m : mobs) {
		if (m->isAlive() && isInRange(*m)) printEntity(base, m);
	}

	//Write status
	stringstream str;
	str
		<< "HP: " << player->getHp()
		<< " | Damage: " << player->getMinDamage() << " - " << player->getMaxDamage()
		<< " | Speed: " << player->getSpeed()
		<< " | Defense: " << player->getDefense();

	line1 = str.str();

	str.str("");
	str
		<< "Turn: " << player->getTurn()
		<< " | Items: " << player->getInventory().size() << "/" <<player->getMaxInventory()
		<< " | Carry: " << player->getCarryWeight() << "/" << player->getMaxCarryWeight();

	line2 = str.str();

	base->str(Point(), status);
	base->str(Point(0, dim.y + 1), line1);
	base->str(Point(0, dim.y + 2), line2);

	//Some terminals have trouble with emoji, so help them out
	//by redrawing the whole window every few frames.
	static int refresh = 0;
	if (emoji && refresh == 5) {
		base->touch();
		refresh = 0;
	}

	base->flip();
	refresh++;
}

void Dungeon::snapshotTake() {
	playerPos = player->getPos();
	playerTurn = player->getTurn();
}

void Dungeon::snapshotRestore() {
	player->setPos(playerPos);
	player->setTurn(playerTurn);
	player->setDungeon(this);
}

const shared_ptr<Mob> Dungeon::getMob(const Point &p) {
	for (const auto& m : mobs) {
		if (!(m->isAlive() && m->getPos() == p)) continue;
		return m;
	}

	return nullptr;
}
