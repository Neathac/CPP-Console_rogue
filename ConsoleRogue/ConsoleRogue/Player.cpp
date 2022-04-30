#include "GameState.h"
#include "libtcod.hpp"
#include "SDL.h"
#include <vector>

void Player::placeSelf(Map& playArea, int x, int y) {
	if (!playArea.isSightBlocker(x, y)) {
		playArea.tiles[this->position[0]][this->position[1]] = this->status; // Leave a character where the player used to be
		this->status = playArea.tiles[x][y];

		playArea.tiles[x][y] = Tileset::player;
		playArea.resetActiveSight();
		playArea.visited[x][y] = 2;
		this->position[0] = x;
		this->position[1] = y;
	}
}

void Player::recalculateActiveSight(Map& playArea) {
	for (int xIndex = -1; xIndex < 2; xIndex += 2) {
		for (int yIndex = -1; yIndex < 2; yIndex += 2) {
			for (auto& vector : this->dirsToCheck) {
				for (auto& arrElement : vector) {
					playArea.visited[this->position[0] + (xIndex*arrElement[0])][this->position[1] + (yIndex * arrElement[1])] = 2;
					if (playArea.isSightBlocker(this->position[0] + (xIndex*arrElement[0]), this->position[1] + (yIndex*arrElement[1]))) {
						break;
					}
				}
			}
		}
	}	
}

void Player::playerInterract(Pickup& pickup) {
	switch (pickup.type) {
	case PICKUP_TYPE::ARMOR:
		this->armor += 1;
		break;
	case PICKUP_TYPE::DAMAGE:
		this->damage += 1;
		break;
	case PICKUP_TYPE::HEALTH_REFILL:
		this->health = this->maxHealth;
		break;
	case PICKUP_TYPE::HEALTH_UPGRADE:
		this->maxHealth += 2;
		break;
	case PICKUP_TYPE::RANGE:
		this->range += 1;
		break;
	case PICKUP_TYPE::SPEED:
		this->speed += 5;
		break;
	}
}