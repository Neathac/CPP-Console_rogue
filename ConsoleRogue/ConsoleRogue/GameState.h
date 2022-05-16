#ifndef GAME_STATE_H
#define GAME_STATE_H

#define CONSOLE_WIDTH 100
#define CONSOLE_HEIGHT 80

#define PLAY_AREA_HEIGHT 60
#define PLAY_AREA_WIDTH 80

#define STAT_AREA_WIDTH 40
#define STAT_AREA_HEIGHT 60

#define EVENT_AREA_WIDTH 100
#define EVENT_AREA_HEIGHT 40

#include "libtcod.hpp"
#include "SDL.h"
// SDL defines main and causes errors
#undef main
#include <vector>
#include <random>
#include <queue>
#include <iostream>

static int getRandomNumber(const int& from, const int& to) {
	int low = from;
	int high = to;
	if (from > to) { // Preventing headaches from misordered parameters
		low = to;
		high = from;
	}
	std::random_device rd; // obtain a random number from hardware
	std::mt19937 gen(rd()); // seed the generator
	std::uniform_int_distribution<> distr(low, high); // define the range
	return distr(gen);
}

enum class DIRECTIONS {
	MOVE_UP,
	MOVE_DOWN,
	MOVE_LEFT,
	MOVE_RIGHT,
};

enum class ROOM_TYPE {
	CORRIDOR,
	CAVE,
	ROOM,
	BLOCKER,
	DISJOINT,
	SAFE_ROOM,
};

// Not treating exit as a pickup would introduce pointless extra complexity. It behaves like one, we just don't randomly generate more
enum class PICKUP_TYPE {
	EXIT = 0,
	DAMAGE = 1,
	ARMOR = 2,
	SPEED = 3,
	HEALTH_REFILL = 4,
	HEALTH_UPGRADE = 5,
	RANGE = 6,
	_count = 7,
};

enum class ACTOR_TYPE {
	GOBLIN = 0,
	UNDETERMINED = 1,
	_count = 2,
};

// Simply store all used characters here for easy global changes
class Tileset {
public:
	static const char wall = '#';
	static const char goblin = 'G';
	static const char player = '@';
	static const char floor = '.';
	static const char exit = 'E';
	static const char damagePickup = 'D';
	static const char speedPickup = 'S';
	static const char armorPickup = 'A';
	static const char healthRefillPickup = 'H';
	static const char healthUpgradePickup = 'M';
	static const char rangePickup = 'R';
private:
	// This class only holds static data
	// Forbid creating its instances
	Tileset() {}
};

class Pickup {
public:
	Pickup(PICKUP_TYPE type, std::array<int, 2> position) : type(type), position(position) {}
	PICKUP_TYPE type;
	std::array<int, 2> position;
};

class Palette {
public:
	Palette(){
		inSightWoodWall = tcod::ColorRGB(102, 51, 0); // Brown #663300
		outOfSightWoodWall = tcod::ColorRGB(77, 38, 0); // Brown #4d2600
		inSightGrassFloor = tcod::ColorRGB(0, 204, 68); // Green #00cc44
		outOfSightGrassFloor = tcod::ColorRGB(0, 102, 34); // Green #006622
		playerCharacter = tcod::ColorRGB(255, 255, 255); // White #ffffff
		statBackground = tcod::ColorRGB(0, 0, 255); // Blue
		statHeaders = tcod::ColorRGB(255, 255, 255); // White #ffffff
		eventBackground = tcod::ColorRGB(255, 255, 255); // White #ffffff
		eventHeaders = tcod::ColorRGB(0, 0, 0); // Black #000000
		inSightPickup = tcod::ColorRGB(255, 255, 0); // Yellow #ffff00
		outOfSightPickup = tcod::ColorRGB(179, 179, 0); // Yellow #b3b300
		goblin = tcod::ColorRGB(255, 0, 0); // Red #ff0000
	}
	tcod::ColorRGB inSightWoodWall;
	tcod::ColorRGB outOfSightWoodWall;
	tcod::ColorRGB inSightGrassFloor;
	tcod::ColorRGB outOfSightGrassFloor;
	tcod::ColorRGB playerCharacter;
	tcod::ColorRGB statBackground;
	tcod::ColorRGB statHeaders;
	tcod::ColorRGB eventBackground;
	tcod::ColorRGB eventHeaders;
	tcod::ColorRGB outOfSightPickup;
	tcod::ColorRGB inSightPickup;
	tcod::ColorRGB goblin;
};

class Room; // Used by RoomGenerator, but Room uses RoomGenerator too
class Player; // Used by Map, but Map uses Player too

class RoomGenerator {
public:
	static void generateSafeRoom(Room& room);
	static void generateDisjointRoom(Room& room);
	static void generateCorridorRoom(Room& room, Room& fromRoom, Room& toRoom, bool hasWalls);
	static void generateCaveRoom(Room& room);
	static void generateBlockerRoom(Room& room);
private:
	RoomGenerator(){} // This class provides only static methods - No need to instantiate it
	static void createWall(Room& room, const std::array<int, 2>& from, const std::array<int, 2>& to, const bool& hasExit);
};

class Actor {
public:
	Actor() {
		position[0] = 5;
		position[1] = 5;
		status = Tileset::floor;
		visible = false;
		health = 5;
		maxHealth = 5;
		speed = 5;
		damage = 5;
		armor = 5;
		range = 5;
		type = ACTOR_TYPE::_count; // Just for initialization
	}
	int position[2];
	char status; // Store tile the entity replaced (floor is underneath the player) - to open way for possible status effects
	bool visible;
	int health;
	int maxHealth;
	int speed;
	int damage;
	int armor;
	int range;
	void moveActor(const int& xChange, const int& yChange);
	ACTOR_TYPE type;
};

class Room {
public:
	Room(const int& roomDiameter, const int& roomCenterX, const int& roomCenterY, ROOM_TYPE roomType) : diameter(roomDiameter), wallPositions(), 
		actorPositions(), pickupPositions(), hazardPositions(), floorPositions() {
		center[0] = roomCenterX;
		center[1] = roomCenterY;
		switch (roomType) {
		case ROOM_TYPE::SAFE_ROOM:
			RoomGenerator::generateSafeRoom(*this);
			break;
		case ROOM_TYPE::DISJOINT:
			RoomGenerator::generateDisjointRoom(*this);
			break;
		case ROOM_TYPE::ROOM:
			RoomGenerator::generateSafeRoom(*this);
			break;
		case ROOM_TYPE::CAVE:
			RoomGenerator::generateSafeRoom(*this);
			break;
		}
	};

	Room(Room& from, Room& to, bool hasWalls) : wallPositions(),
		actorPositions(), pickupPositions(), hazardPositions(), floorPositions() {
		RoomGenerator::generateCorridorRoom(*this ,from, to, hasWalls);
	};
	int diameter;
	std::array<int, 2> center;
	std::vector<std::array<int, 2>> wallPositions;
	std::vector<std::array<int, 2>> actorPositions;
	std::vector<std::array<int, 2>> pickupPositions;
	std::vector<std::array<int, 2>> hazardPositions;
	std::vector<std::array<int, 2>> floorPositions;
};

// Simply stores current level metadata for better modularity
class Level {
public:
	Level(tcod::ColorRGB inSightWall, const tcod::ColorRGB& outOfSightWall,
		const tcod::ColorRGB& inSightFloor, const tcod::ColorRGB& outOfSightFloor, 
		const tcod::ColorRGB& outOfSightPickup, const tcod::ColorRGB& inSightPickup, const int& difficulty) :
		inSightWall(inSightWall), // Known bug - The saved value isn't once anywhere in the whole project for some reason
		inSightFloor(inSightFloor), 
		outOfSightWall(outOfSightWall),
		outOfSightFloor(outOfSightFloor),
		difficultyLevel(difficulty),
		inSightPickup(inSightPickup),
		outOfSightPickup(outOfSightPickup)
	{
			int xPolarity;
			if (getRandomNumber(1,2)%2 == 0) {
				xPolarity = -1;
			}
			else {
				xPolarity = 1;
			}
			int yPolarity;
			if (getRandomNumber(1, 2) % 2 == 0) {
				yPolarity = -1;
			}
			else {
				yPolarity = 1;
			}

			safeRoom = new Room(4, PLAY_AREA_WIDTH/2, PLAY_AREA_HEIGHT/2, ROOM_TYPE::SAFE_ROOM); // Safe room is always the same
			exitRoom = new Room(4, (PLAY_AREA_WIDTH / 2) + xPolarity*getRandomNumber(9, (PLAY_AREA_WIDTH / 2)-5), (PLAY_AREA_HEIGHT / 2) + yPolarity * getRandomNumber(9, (PLAY_AREA_HEIGHT / 2) - 5), ROOM_TYPE::SAFE_ROOM);
			
			if (difficultyLevel < 3) {
				corridors.push_back(new Room(*safeRoom, *exitRoom, false));
				this->generateEasyEnvironment();
			}
/*			else if (difficultyLevel > 2 && difficultyLevel < 6) {
				this->generateMediumEnvironment();
			}
			else if (difficultyLevel > 5) {
				this->generateDifficultEnvironment();
			}
			*/
	}
	void generateEasyEnvironment();
	void generateMediumEnvironment();
	void generateDifficultEnvironment();
	int difficultyLevel;
	Room* safeRoom;
	Room* exitRoom;
	std::vector<Room*> rooms;
	std::vector<Room*> corridors;
	std::vector<Pickup*> pickups;
	std::vector<Actor*> hostileActors;
	const tcod::ColorRGB& inSightWall;
	const tcod::ColorRGB& outOfSightWall;
	const tcod::ColorRGB& inSightFloor;
	const tcod::ColorRGB& outOfSightFloor;
	const tcod::ColorRGB& inSightPickup;
	const tcod::ColorRGB& outOfSightPickup;
private:
	void populatePickups();
	void populateEnemies();
};

class Map {
public:
	Map(const std::shared_ptr<Palette> palette) : palette(palette), level(new Level(palette->inSightWoodWall,
		palette->outOfSightWoodWall, palette->inSightGrassFloor, palette->outOfSightGrassFloor, palette->outOfSightPickup, palette->inSightPickup, 1)) // The last argument always instantiates level 1 environment
	{
		sightBlockers = { Tileset::wall, Tileset::armorPickup, Tileset::damagePickup, Tileset::exit, Tileset::healthRefillPickup,
			Tileset::healthUpgradePickup, Tileset::rangePickup, Tileset::speedPickup };
	}
	void setupNewPlayArea(Player& player, tcod::Console& console, tcod::ContextPtr& context);
	void drawWholeMap(tcod::Console& console, tcod::ContextPtr& context);
	void setSingleTile(tcod::Console& console, const int& x, const int& y);
	bool isSightBlocker(const int& x, const int& y);
	void resetActiveSight();
	void generateNewLevel(const int& difficultyLevel);
	void drawRooms();
	void drawPickups();
	void drawEnemies();

	int visited[PLAY_AREA_WIDTH][PLAY_AREA_HEIGHT];
	char tiles[PLAY_AREA_WIDTH][PLAY_AREA_HEIGHT];
	std::vector<char> sightBlockers;
	std::shared_ptr<Palette> palette;
	Level* level;
};

class Player : public Actor {
public:
	Player() {
		dirsToCheck.push_back(std::vector < std::array<int, 2>>{
			std::array<int, 2>{0, 1}, std::array<int, 2>{0, 2}, std::array<int, 2>{0, 3}, std::array<int, 2>{0, 4}, std::array<int, 2>{0, 5}});
		dirsToCheck.push_back(std::vector < std::array<int, 2>>{
			std::array<int, 2>{0, 1}, std::array<int, 2>{1, 1}, std::array<int, 2>{1, 2}, std::array<int, 2>{1, 3}, std::array<int, 2>{1, 4}});
		dirsToCheck.push_back(std::vector < std::array<int, 2>>{
			std::array<int, 2>{0, 1}, std::array<int, 2>{1, 1}, std::array<int, 2>{1, 2}, std::array<int, 2>{2, 2}, std::array<int, 2>{2, 3}});
		dirsToCheck.push_back(std::vector < std::array<int, 2>>{
			std::array<int, 2>{1, 0}, std::array<int, 2>{1, 1}, std::array<int, 2>{2, 1}, std::array<int, 2>{2, 2}, std::array<int, 2>{3, 2}});
		dirsToCheck.push_back(std::vector < std::array<int, 2>>{
			std::array<int, 2>{1, 0}, std::array<int, 2>{1, 1}, std::array<int, 2>{2, 1}, std::array<int, 2>{3, 1}, std::array<int, 2>{4, 1}});
		dirsToCheck.push_back(std::vector < std::array<int, 2>>{
			std::array<int, 2>{1, 0}, std::array<int, 2>{2, 0}, std::array<int, 2>{3, 0}, std::array<int, 2>{4, 0}, std::array<int, 2>{5, 0}});
		status = Tileset::floor;
		visible = true;
	}
	void placeSelf(Map& playArea, int x, int y);
	void recalculateActiveSight(Map& playArea);
	void playerAttack(Actor& actor);
	void playerInterract(Pickup& pickup);
	
private:
	std::vector<std::vector<std::array<int, 2>>> dirsToCheck;
	
};

class PlayerStatSection {
public:
	PlayerStatSection(const std::shared_ptr<Palette> palette) : palette(palette) {}
	void setPlayer(const std::shared_ptr<Player> player) { this->player = player; }
	void colorArea(tcod::Console& console, tcod::ContextPtr& context);
	void drawTextFields(tcod::Console& console, tcod::ContextPtr& context);
	void drawStatValues(tcod::Console& console, tcod::ContextPtr& context);
private:
	std::shared_ptr<Palette> palette;
	std::shared_ptr<Player> player;
};

class EventSection {
public:
	EventSection(const std::shared_ptr<Palette> palette) : palette(palette) {}
	void colorArea(tcod::Console& console, tcod::ContextPtr& context);
	void newEvent(tcod::Console& console, tcod::ContextPtr& context, std::string eventDescription);
private:
	// TODO: Make types of events so they could be drawn in different colors?
	// Could possibly remove the need for a palette pointer
	void drawEvents(tcod::Console& console, tcod::ContextPtr& context);
	std::shared_ptr<Palette> palette;
	std::queue<std::string> events;
};

class Game {
public:
	Game(const std::shared_ptr<Palette> palette) :  player(new Player()), playArea(palette), statSection(palette), eventSection(palette) {

		console = tcod::Console{ CONSOLE_WIDTH, CONSOLE_HEIGHT };  // Main console.

		// Configure the context.
		auto params = TCOD_ContextParams{};
		params.tcod_version = TCOD_COMPILEDVERSION;  // This is required.
		params.console = console.get();  // Derive the window size from the console size.
		params.window_title = "Console Rogue";
		params.sdl_window_flags = SDL_WINDOW_SHOWN;
		params.vsync = true;
		context = tcod::new_context(params);

		// Initialise and sketch out Stat section of console
		statSection.setPlayer(player);
		statSection.colorArea(console, context);
		statSection.drawTextFields(console, context);
		statSection.drawStatValues(console, context);

		//Initialise play area
		playArea.setupNewPlayArea(*player ,console, context);
		

		// Instantiate player vision
		// This does not cause the bug to manifest
		player->recalculateActiveSight(playArea);
		playArea.drawWholeMap(console, context);

		// Initialise eventArea
		eventSection.colorArea(console, context);
	}
	void playerMove(DIRECTIONS direction);
	void playerInterract();
private:
	tcod::Console console;
	tcod::ContextPtr context;
	Map playArea;
	std::shared_ptr<Player> player;
	PlayerStatSection statSection;
	EventSection eventSection;
};

#endif 