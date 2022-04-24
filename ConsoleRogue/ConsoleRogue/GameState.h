#ifndef GAME_STATE_H
#define GAME_STATE_H

#define PLAY_AREA_WIDTH 60
#define PLAY_AREA_HEIGHT 80
#define CONSOLE_WIDTH 100
#define CONSOLE_HEIGHT 80
#define STAT_AREA_WIDTH 40
#define STAT_AREA_HEIGHT 80

#include "libtcod.hpp"
#include "SDL.h"
// SDL defines main and causes errors
#undef main
#include <vector>
#include <random>

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
		inSightWall = tcod::ColorRGB(255, 0, 0); // Red #ff0000
		outOfSightWall = tcod::ColorRGB(128, 0, 0); // Red #800000
		inSightFloor = tcod::ColorRGB(255, 255, 255); // White #ffffff
		outOfSightFloor = tcod::ColorRGB(128, 128, 128); // White #808080
		playerCharacter = tcod::ColorRGB(255, 255, 255); // White #ffffff
		statBackground = tcod::ColorRGB(0, 0, 255);
	}
	tcod::ColorRGB inSightWall;
	tcod::ColorRGB outOfSightWall;
	tcod::ColorRGB inSightFloor;
	tcod::ColorRGB outOfSightFloor;
	tcod::ColorRGB playerCharacter;
	tcod::ColorRGB statBackground;
};

class Map {
public:
	Map(){}
	Map(const Palette& palette) : palette(std::make_unique<Palette>(palette)) {
		sightBlockers = { Tileset::wall };
		setupNewPlayArea();
	}
	void setupNewPlayArea();
	void drawWholeMap(tcod::Console& console, tcod::ContextPtr& context);
	void setSingleTile(tcod::Console& console, const int& x, const int& y);
	void generateNewLevel();
	bool isSightBlocker(const int& x, const int& y);
	void resetActiveSight();

	int visited[PLAY_AREA_HEIGHT][PLAY_AREA_WIDTH];
	char tiles[PLAY_AREA_HEIGHT][PLAY_AREA_WIDTH];
	std::vector<char> sightBlockers;
	std::unique_ptr<Palette> palette;
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
	PlayerStatSection(){}
	PlayerStatSection(const Palette& palette, const Player& player) : palette(std::make_unique<Palette>(palette)), player(std::make_unique<Player>(player)) {}
private:
	std::unique_ptr<Palette> palette;
	std::unique_ptr<Player> player;
	void colorArea(tcod::Console& console, tcod::ContextPtr& context);
};

class Game {
public:
	Game() : palette(), player() {
		playArea = new Map(palette);
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
	std::vector<Actor> entities;
	Palette palette;
};

#endif 

