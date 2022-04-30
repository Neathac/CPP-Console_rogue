#include "GameState.h"
#include "libtcod.hpp"
#include "SDL.h"
#include <string>
// SDL defines main and causes errors
#undef main

using namespace std;

void Map::setupNewPlayArea(Player& player, tcod::Console& console, tcod::ContextPtr& context) {
	// Set up play area borders
	for (int i = 0; i < PLAY_AREA_HEIGHT; ++i) {
		this->visited[0][i] = 0;
		this->visited[PLAY_AREA_WIDTH -1][i] = 0;
		this->tiles[0][i] = Tileset::wall;
		this->tiles[PLAY_AREA_WIDTH - 1][i] = Tileset::wall;
	}
	for (int i = 0; i < PLAY_AREA_WIDTH; ++i) {
		this->visited[i][0] = 0;
		this->visited[i][PLAY_AREA_HEIGHT - 1] = 0;
		this->tiles[i][0] = Tileset::wall;
		this->tiles[i][PLAY_AREA_HEIGHT - 1] = Tileset::wall;
	}
	// Fill the map with floor
	for (int i = 1; i < PLAY_AREA_WIDTH - 1; ++i) {
		for (int j = 1; j < PLAY_AREA_HEIGHT - 1; ++j) {
			this->visited[i][j] = 1;
			this->tiles[i][j] = Tileset::floor;
		}
	}
	player.placeSelf(*this, this->level->safeRoom->center[0], this->level->safeRoom->center[1]);
	this->drawRooms();
	this->drawPickups();
	drawWholeMap(console, context);
	return;
}

void Map::drawWholeMap(tcod::Console& console, tcod::ContextPtr& context) {
	for (int i = 0; i < PLAY_AREA_WIDTH; ++i) {
		for (int j = 0; j < PLAY_AREA_HEIGHT; ++j) {
			this->setSingleTile(console, i, j);
		}
	}
	context->present(console);
}

void Map::setSingleTile(tcod::Console& console, const int& x, const int& y) {
	std::string toPrint = "";
	toPrint += tiles[x][y];
	// The differentiation is necesarry for differences in behavior for tile in active FOV
	switch (this->tiles[x][y]) {
	case Tileset::wall:
		if (this->visited[x][y] == 2) {	tcod::print(console, { x,y }, toPrint, level->inSightWall , std::nullopt); }
		else if (this->visited[x][y] == 1) { tcod::print(console, { x,y }, toPrint, level->outOfSightWall, std::nullopt); }
		break;
	case Tileset::floor:
		if (this->visited[x][y] == 2) { tcod::print(console, { x,y }, toPrint, level->inSightFloor, std::nullopt); }
		else if (this->visited[x][y] == 1) { tcod::print(console, { x,y }, toPrint, level->outOfSightFloor, std::nullopt); }
		break;
	case Tileset::player:
		tcod::print(console, { x,y }, toPrint, palette->playerCharacter, std::nullopt);
		break;
	default: // Pickups
		if (this->visited[x][y] == 2) { tcod::print(console, { x,y }, toPrint, level->inSightPickup, std::nullopt); }
		else if (this->visited[x][y] == 1) { tcod::print(console, { x,y }, toPrint, level->outOfSightPickup, std::nullopt); }
		break;
	}
}

bool Map::isSightBlocker(const int& x, const int& y) {
	if (x < 0 || x > PLAY_AREA_WIDTH - 1 || y < 0 || y > PLAY_AREA_HEIGHT - 1) {
		return true;
	}
	else if (std::find(this->sightBlockers.begin(), this->sightBlockers.end(), this->tiles[x][y]) != this->sightBlockers.end()) {
		return true;
	}
	return false;
}

void Map::resetActiveSight() {
	for (int i = 0; i < PLAY_AREA_WIDTH; ++i) {
		for (int j = 0; j < PLAY_AREA_HEIGHT; ++j) {
			if (this->visited[i][j] == 2) { this->visited[i][j] = 1; };
		}
	}
}

void Map::generateNewLevel(const int& difficultyLevel) {
	// We call the level constructor again
	if (difficultyLevel < 3) {
		this->level = new Level(palette->inSightWoodWall,
			palette->outOfSightWoodWall, palette->inSightGrassFloor, palette->outOfSightGrassFloor, palette->outOfSightPickup, palette->inSightPickup, difficultyLevel);
	}
	else if (difficultyLevel > 2 && difficultyLevel < 6) {
		this->level = new Level(palette->inSightWoodWall,
			palette->outOfSightWoodWall, palette->inSightGrassFloor, palette->outOfSightGrassFloor, palette->outOfSightPickup, palette->inSightPickup, difficultyLevel);
	}
	else if (difficultyLevel > 5) {
		this->level = new Level(palette->inSightWoodWall,
			palette->outOfSightWoodWall, palette->inSightGrassFloor, palette->outOfSightGrassFloor, palette->outOfSightPickup, palette->inSightPickup, difficultyLevel);
	}
}

void Map::drawRooms() {
	for (auto &coord : this->level->safeRoom->wallPositions) {
		this->tiles[coord[0]][coord[1]] = Tileset::wall;
	}
	for (auto& coord : this->level->exitRoom->wallPositions) {
		this->tiles[coord[0]][coord[1]] = Tileset::wall;
	}
	for (auto& room : this->level->rooms) {
		for (auto& coord : room->wallPositions) {
			this->tiles[coord[0]][coord[1]] = Tileset::wall;
		}
	}
	for (auto& room : this->level->corridors) {
		for (auto& coord : room->wallPositions) {
			this->tiles[coord[0]][coord[1]] = Tileset::wall;
		}
		for (auto& coord : room->floorPositions) {
			this->tiles[coord[0]][coord[1]] = Tileset::floor;
		}
	}
}

void Map::drawPickups() {
	for (auto& pickup : this->level->pickups) {
		switch (pickup->type) {
		case PICKUP_TYPE::EXIT:
			tiles[pickup->position[0]][pickup->position[1]] = Tileset::exit;
			break;
		case PICKUP_TYPE::ARMOR:
			tiles[pickup->position[0]][pickup->position[1]] = Tileset::armorPickup;
			break;
		case PICKUP_TYPE::DAMAGE:
			tiles[pickup->position[0]][pickup->position[1]] = Tileset::damagePickup;
			break;
		case PICKUP_TYPE::HEALTH_REFILL:
			tiles[pickup->position[0]][pickup->position[1]] = Tileset::healthRefillPickup;
			break;
		case PICKUP_TYPE::HEALTH_UPGRADE:
			tiles[pickup->position[0]][pickup->position[1]] = Tileset::healthUpgradePickup;
			break;
		case PICKUP_TYPE::RANGE:
			tiles[pickup->position[0]][pickup->position[1]] = Tileset::rangePickup;
			break;
		case PICKUP_TYPE::SPEED:
			tiles[pickup->position[0]][pickup->position[1]] = Tileset::speedPickup;
			break;
		}
	}
}