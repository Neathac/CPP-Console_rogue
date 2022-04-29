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

// Simply store all used characters here for easy global changes
class Tileset {
public:
	static const char wall = '#';
	static const char player = '@';
	static const char floor = '.';
private:
	// This class only holds static data
	// Forbid creating its instances
	Tileset() {}
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
		const tcod::ColorRGB& inSightFloor, const tcod::ColorRGB& outOfSightFloor, const int& difficulty) : 
		inSightWall(inSightWall), // Known bug - The saved value isn't once anywhere in the whole project for some reason
		inSightFloor(inSightFloor), 
		outOfSightWall(outOfSightWall),
		outOfSightFloor(outOfSightFloor),
		difficultyLevel(difficulty) {
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
			corridors.push_back(new Room(*safeRoom, *exitRoom, true));
			if (difficultyLevel < 3) {
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
	const tcod::ColorRGB& inSightWall;
	const tcod::ColorRGB& outOfSightWall;
	const tcod::ColorRGB& inSightFloor;
	const tcod::ColorRGB& outOfSightFloor;
};

class Map {
public:
	Map(const Palette& palette) : palette(std::make_unique<Palette>(palette)), level(new Level(palette.inSightWoodWall,
		palette.outOfSightWoodWall, palette.inSightGrassFloor, palette.outOfSightGrassFloor, 1)) // The last argument always instantiates level 1 environment
	{
		sightBlockers = { Tileset::wall };
	}
	void setupNewPlayArea(Player& player, tcod::Console& console, tcod::ContextPtr& context);
	void drawWholeMap(tcod::Console& console, tcod::ContextPtr& context);
	void setSingleTile(tcod::Console& console, const int& x, const int& y);
	bool isSightBlocker(const int& x, const int& y);
	void resetActiveSight();
	void generateNewLevel(const int& difficultyLevel);
	void drawRooms();

	int visited[PLAY_AREA_WIDTH][PLAY_AREA_HEIGHT];
	char tiles[PLAY_AREA_WIDTH][PLAY_AREA_HEIGHT];
	std::vector<char> sightBlockers;
	std::unique_ptr<Palette> palette;
	Level* level;
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
	void playerAction(Map& playArea);
	
private:
	std::vector<std::vector<std::array<int, 2>>> dirsToCheck;
	
};

class PlayerStatSection {
public:
	PlayerStatSection(const Palette& palette) : palette(std::make_unique<Palette>(palette)) {}
	void setPlayer(const Player& player) { this->player = std::make_unique<Player>(player); }
	void colorArea(tcod::Console& console, tcod::ContextPtr& context);
	void drawTextFields(tcod::Console& console, tcod::ContextPtr& context);
	void drawStatValues(tcod::Console& console, tcod::ContextPtr& context);
private:
	std::unique_ptr<Palette> palette;
	std::unique_ptr<Player> player;
};

class EventSection {
public:
	EventSection(const Palette& palette) : palette(std::make_unique<Palette>(palette)) {}
	void colorArea(tcod::Console& console, tcod::ContextPtr& context);
	void newEvent(tcod::Console& console, tcod::ContextPtr& context, std::string eventDescription);
private:
	// TODO: Make types of events so they could be drawn in different colors?
	// Could possibly remove the need for a palette pointer
	void drawEvents(tcod::Console& console, tcod::ContextPtr& context);
	std::unique_ptr<Palette> palette;
	std::queue<std::string> events;
};

class Game {
public:
	Game(const Palette& palette) :  player(), playArea(palette), statSection(palette), eventSection(palette) {

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
		playArea.setupNewPlayArea(player,console, context);

		// Initialise eventArea
		eventSection.colorArea(console, context);
	}
	void playerMove(DIRECTIONS direction);
	void updateEntityActivity(); // TODO: On player action, update visible entities
private:
	tcod::Console console;
	tcod::ContextPtr context;
	Map playArea;
	Player player;
	PlayerStatSection statSection;
	EventSection eventSection;
	std::vector<Actor> entities;
};

#endif 

