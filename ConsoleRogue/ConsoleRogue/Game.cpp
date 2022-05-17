#include "GameState.h"

void Game::playerMove(DIRECTIONS direction) {
	switch (direction) {
	case DIRECTIONS::MOVE_DOWN:
		this->player->placeSelf(this->playArea, this->player->position[0], this->player->position[1] + 1);
		break;
	case DIRECTIONS::MOVE_UP:
		this->player->placeSelf(this->playArea, this->player->position[0], this->player->position[1] - 1);
		break;
	case DIRECTIONS::MOVE_LEFT:
		this->player->placeSelf(this->playArea, this->player->position[0] - 1, this->player->position[1]);
		break;
	case DIRECTIONS::MOVE_RIGHT:
		this->player->placeSelf(this->playArea, this->player->position[0] + 1, this->player->position[1]);
		break;
	}
	// This breaks walls in active sight, but the exact same thing in the interraction function does not
	// Except the very first move. That one does not break walls in active sight
	this->player->recalculateActiveSight(this->playArea);
	this->playArea.level->updateEnemies(this->playArea, this->player);
	this->statSection.drawStatValues(this->console, this->context);
	this->player->recalculateActiveSight(this->playArea);
	this->playArea.drawWholeMap(this->console, this->context);
}

void Game::playerInterract() {
	bool interractionOccured = false;
	if (!interractionOccured) {
		for (int i = 0; i < this->playArea.level->hostileActors.size(); ++i) { // We will be erasing elements from vector by position, so we need the index
			if ((abs(this->playArea.level->hostileActors[i]->position[0] - this->player->position[0]) <= this->player->range) &&
				(abs(this->playArea.level->hostileActors[i]->position[1] - this->player->position[1]) <= this->player->range) &&
				(this->playArea.visited[this->playArea.level->hostileActors[i]->position[0]][this->playArea.level->hostileActors[i]->position[1]] == 2)) { // Enemy is both within range and in line of sight
				this->playArea.level->hostileActors[i]->health -= (*player).damage - (this->playArea.level->hostileActors[i]->armor/2);
				interractionOccured = true;
				if (this->playArea.level->hostileActors[i]->health < 1) { // Actor was killed
					this->playArea.tiles[this->playArea.level->hostileActors[i]->position[0]][this->playArea.level->hostileActors[i]->position[1]] = Tileset::floor;
					this->playArea.level->hostileActors.erase(this->playArea.level->hostileActors.begin() + i);
				}
				break; // Only one interraction per action premitted
			}
		}
	}
	
	if (!interractionOccured) {
		// No enemies in sight were found, check for pickups
		for (int i = 0; i < this->playArea.level->pickups.size(); ++i) { // We will be erasing elements from vector by position, so we need the index
			if ((abs(this->playArea.level->pickups[i]->position[0] - this->player->position[0]) <= this->player->range) &&
				(abs(this->playArea.level->pickups[i]->position[1] - this->player->position[1]) <= this->player->range) &&
				(this->playArea.visited[this->playArea.level->pickups[i]->position[0]][this->playArea.level->pickups[i]->position[1]] == 2)) { // Pickup is both within range and in line of sight
				interractionOccured = true;
				this->player->playerInterract(*this->playArea.level->pickups[i]);
				this->playArea.tiles[this->playArea.level->pickups[i]->position[0]][this->playArea.level->pickups[i]->position[1]] = Tileset::floor;
				this->playArea.level->pickups.erase(this->playArea.level->pickups.begin()+i); // Pickups are one-time use only
				break; // Only one interraction per action premitted
			}
		}
	}
	this->statSection.drawStatValues(this->console, this->context);
	this->playArea.level->updateEnemies(this->playArea, this->player);
	this->statSection.drawStatValues(this->console, this->context);
	this->player->recalculateActiveSight(this->playArea);
	this->playArea.drawWholeMap(this->console, this->context);
}