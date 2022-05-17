#include "GameState.h"
#include "libtcod.hpp"
#include "SDL.h"
#include <vector>

// TODO: Create a balancing struct containing all the values to be plugged into generating new levels for better modularity
void Level::generateEasyEnvironment() {
	int lowLimitOfRooms = PLAY_AREA_WIDTH / 7; // Entirely arbitrary
	int highLimitOfRooms = lowLimitOfRooms * (PLAY_AREA_HEIGHT / 10); // The idea is to put limits as if we wanted to fill the entire play area by 5*5 rooms
	
	int numOfRooms = getRandomNumber(lowLimitOfRooms, highLimitOfRooms);

	std::vector<int> diameters;
	std::vector<std::array<int, 2>> centers;

	int xOffset = 1;
	int yOffset = 1;

	for (int i = 0; i < numOfRooms; ++i) {
		int newDiameter = getRandomNumber(2, 6); // Trees should be on the shorter side

		// Sizes of new rooms
		diameters.push_back(newDiameter);

		// Centers of new rooms
		xOffset = (xOffset + (newDiameter*getRandomNumber(2,7))) % PLAY_AREA_WIDTH; // These offsets will serve as room centers
		yOffset = (yOffset + (newDiameter * getRandomNumber(2, 7))) % PLAY_AREA_HEIGHT;

		// Prevent rooms going off-bounds, or colliding with exit or spawn rooms
		while (xOffset < 1 + newDiameter || xOffset + newDiameter >= PLAY_AREA_WIDTH || 
			(abs(xOffset-(this->safeRoom->center[0])) < 5+newDiameter) && (abs(yOffset - (this->safeRoom->center[1])) < 5 + newDiameter) ||
			(abs(xOffset - (this->exitRoom->center[0])) < 5 + newDiameter) && (abs(yOffset - (this->exitRoom->center[1])) < 5 + newDiameter)) { // Make sure we don't go off-bounds
			xOffset = (xOffset + (newDiameter * getRandomNumber(2, 7))) % PLAY_AREA_WIDTH; // The value is bound to not overlap eventually
		}
		// Same as above, but for y coordinate
		while (yOffset < 1 + newDiameter || yOffset + newDiameter >= PLAY_AREA_HEIGHT ||
			(abs(xOffset - (this->safeRoom->center[0])) < 5 + newDiameter) && (abs(yOffset - (this->safeRoom->center[1])) < 5 + newDiameter) ||
			(abs(xOffset - (this->exitRoom->center[0])) < 5 + newDiameter) && (abs(yOffset - (this->exitRoom->center[1])) < 5 + newDiameter)) { // Make sure we don't go off-bounds
			yOffset = (yOffset + (newDiameter * getRandomNumber(2, 7))) % PLAY_AREA_WIDTH;
		}
		centers.push_back({ xOffset, yOffset });
	}

	// Disjoint rooms are meant to emulate trees in a forest. They are just two walls with possible positions for enemies and pickups
	for (int i = 0; i < diameters.size(); ++i) {
		this->rooms.push_back(new Room(diameters[i], centers[i][0], centers[i][1], ROOM_TYPE::DISJOINT));
	}
	populatePickups();
	populateEnemies(4, int(ACTOR_TYPE::GOBLIN));
}

void Level::populatePickups() {
	for (auto& room : rooms) {
		if (getRandomNumber(1, 4) == 1) { // Roughly 1/3 rooms should have pickups
			for (auto& coords : room->pickupPositions) {
				int pickup = getRandomNumber(int(PICKUP_TYPE::DAMAGE), int(PICKUP_TYPE::_count) - 1 );
				if (pickup == int(PICKUP_TYPE::RANGE)) { // More range is pretty overpowered, so we make it very rare
					pickup = getRandomNumber(int(PICKUP_TYPE::DAMAGE), int(PICKUP_TYPE::_count) - 1 );
				}
				this->pickups.push_back(new Pickup(PICKUP_TYPE(pickup), coords));
			}
			
		}
	}
}

void Level::populateEnemies(const int& spawnRate, const int& rangeOfEnemies) {
	for (auto& room : rooms) {
		if (getRandomNumber(1, spawnRate) == 1) {
			for (auto& coords : room->actorPositions) {
				int enemy = getRandomNumber(int(ACTOR_TYPE::GOBLIN), rangeOfEnemies);
				this->hostileActors.push_back(new Actor(ACTOR_TYPE(enemy), coords));
			}

		}
	}
}

void Level::updateEnemies(Map& map, std::shared_ptr<Player> player) {
	for (auto& enemy : this->hostileActors) {
		enemy->speed += player->speed;
		if (enemy->speed + player->speed >= enemy->speedLimit) { // The enemy is allowed to move
			enemy->speed = enemy->speed % enemy->speedLimit; // Reset the enemy movement
			if (map.visited[enemy->position[0]][enemy->position[1]] == 2) { // The enemy is in FOV
				if ((abs(enemy->position[0] - player->position[0]) <= enemy->range) &&
					(abs(enemy->position[1] - player->position[1]) <= enemy->range)) { // The enemy can reach the player
					player->health -= enemy->damage / player->armor;
				}
				else { // Player not in reach, move towards him
					if ((player->position[0] - enemy->position[0]) > 0 && // Try to align horizontally first
						!map.isSightBlocker(enemy->position[0] + 1 ,enemy->position[1])) { // Make sure we don't go into a wall
						map.tiles[enemy->position[0]][enemy->position[1]] = Tileset::floor;
						enemy->position[0] += 1;
					}
					else if ((player->position[0] - enemy->position[0]) < 0 && // Try to align horizontally first
						!map.isSightBlocker(enemy->position[0] - 1, enemy->position[1])) {
						map.tiles[enemy->position[0]][enemy->position[1]] = Tileset::floor;
						enemy->position[0] -= 1;
					}
					else { // We need to move vertically
						if ((player->position[1] - enemy->position[1]) > 0 && // Try to align horizontally first
							!map.isSightBlocker(enemy->position[0], enemy->position[1] + 1)) { // Make sure we don't go into a wall
							map.tiles[enemy->position[0]][enemy->position[1]] = Tileset::floor;
							enemy->position[1] += 1;
						}
						else if ((player->position[1] - enemy->position[1]) < 0 && // Try to align horizontally first
							!map.isSightBlocker(enemy->position[0], enemy->position[1] - 1)) {
							map.tiles[enemy->position[0]][enemy->position[1]] = Tileset::floor;
							enemy->position[1] -= 1;
						}
					}
				}
			}		
		}
	}
}