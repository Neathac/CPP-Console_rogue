#include "GameState.h"
#include "libtcod.hpp"
#include "SDL.h"
#include <vector>

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
		// Prevent rooms going off-bounds, or coliding with exit or spawn rooms
		while (xOffset < 1 + newDiameter || xOffset + newDiameter >= PLAY_AREA_WIDTH || 
			(abs(xOffset-(this->safeRoom->center[0])) < 5+newDiameter) && (abs(yOffset - (this->safeRoom->center[1])) < 5 + newDiameter) ||
			(abs(xOffset - (this->exitRoom->center[0])) < 5 + newDiameter) && (abs(yOffset - (this->exitRoom->center[1])) < 5 + newDiameter)) { // Make sure we don't go off-bounds
			xOffset = (xOffset + (newDiameter * getRandomNumber(2, 7))) % PLAY_AREA_WIDTH;
		}
		while (yOffset < 1 + newDiameter || yOffset + newDiameter >= PLAY_AREA_HEIGHT ||
			(abs(xOffset - (this->safeRoom->center[0])) < 5 + newDiameter) && (abs(yOffset - (this->safeRoom->center[1])) < 5 + newDiameter) ||
			(abs(xOffset - (this->exitRoom->center[0])) < 5 + newDiameter) && (abs(yOffset - (this->exitRoom->center[1])) < 5 + newDiameter)) { // Make sure we don't go off-bounds
			yOffset = (yOffset + (newDiameter * getRandomNumber(2, 7))) % PLAY_AREA_WIDTH;
		}
		centers.push_back({ xOffset, yOffset });
	}
	for (int i = 0; i < diameters.size(); ++i) {
		this->rooms.push_back(new Room(diameters[i], centers[i][0], centers[i][1], ROOM_TYPE::DISJOINT));
	}
}