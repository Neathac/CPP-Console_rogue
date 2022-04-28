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

enum DIRECTIONS {
	MOVE_UP,
	MOVE_DOWN,
	MOVE_LEFT,
	MOVE_RIGHT,
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

class Room {

};

// Simply stores current level metadata for better modularity
class Level {
public:
	Level(tcod::ColorRGB inSightWall, const tcod::ColorRGB& outOfSightWall,
		const tcod::ColorRGB& inSightFloor, const tcod::ColorRGB& outOfSightFloor) : 
		inSightWall(inSightWall),
		inSightFloor(inSightFloor), 
		outOfSightWall(outOfSightWall),
		outOfSightFloor(outOfSightFloor) {
		std::cout << inSightWall.r << std::endl;
	}
	const tcod::ColorRGB& inSightWall;
	const tcod::ColorRGB& outOfSightWall;
	const tcod::ColorRGB& inSightFloor;
	const tcod::ColorRGB& outOfSightFloor;
};

class Map {
public:
	Map(const Palette& palette) : palette(std::make_unique<Palette>(palette)), level(new Level(palette.inSightWoodWall,
		palette.outOfSightWoodWall, palette.inSightGrassFloor, palette.outOfSightGrassFloor)) {
		sightBlockers = { Tileset::wall };
		setupNewPlayArea();
	}
	void setupNewPlayArea();
	void drawWholeMap(tcod::Console& console, tcod::ContextPtr& context);
	void setSingleTile(tcod::Console& console, const int& x, const int& y);
	bool isSightBlocker(const int& x, const int& y);
	void resetActiveSight();
	void generateNewLevel(const int& difficultyLevel);
	void createRoom(const int& x, const int& y);
	void connectAdjecentRooms();

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

		// Initialise all tiles
		playArea.setupNewPlayArea();

		// Initialise and sketch out Stat section of console
		statSection.setPlayer(player);
		statSection.colorArea(console, context);
		statSection.drawTextFields(console, context);
		statSection.drawStatValues(console, context);

		// Initialise eventArea
		eventSection.colorArea(console, context);

		// Place the player in the map
		player.placeSelf(playArea, 5, 5);
		player.recalculateActiveSight(playArea);

		// Render the entire map
		playArea.drawWholeMap(console, context);
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

