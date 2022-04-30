#include "GameState.h"
#include "libtcod.hpp"
#include "SDL.h"

void RoomGenerator::generateSafeRoom(Room& room) {
	createWall(room, { room.center[0] + room.diameter, room.center[1] - room.diameter }, { room.center[0] + room.diameter, room.center[1] + room.diameter }, true);
	createWall(room, { room.center[0] - room.diameter, room.center[1] + room.diameter }, { room.center[0] + room.diameter, room.center[1] + room.diameter }, true);
	createWall(room, { room.center[0] - room.diameter, room.center[1] - room.diameter }, { room.center[0] + room.diameter, room.center[1] - room.diameter }, true);
	createWall(room, { room.center[0] - room.diameter, room.center[1] - room.diameter }, { room.center[0] - room.diameter, room.center[1] + room.diameter }, true);
	room.pickupPositions.push_back(room.center);
}

void RoomGenerator::generateDisjointRoom(Room& room) {
	createWall(room, { room.center[0] + room.diameter/2, room.center[1] - room.diameter/2 }, { room.center[0] + room.diameter/2, room.center[1] + room.diameter/2 }, false);
	createWall(room, { room.center[0] - room.diameter, room.center[1] - room.diameter }, { room.center[0] - room.diameter, room.center[1] + room.diameter }, true);
	room.actorPositions.push_back({ room.center[0] + room.diameter, room.center[1] - room.diameter });
	room.actorPositions.push_back({ room.center[0] - room.diameter, room.center[1] - room.diameter });
	room.pickupPositions.push_back(room.center);
}

void RoomGenerator::generateCorridorRoom(Room& room, Room& from, Room& to, bool hasWalls) {
	bool doXOverlap = ((from.center[0]-from.diameter-2) <= (to.center[0] + to.diameter-2) &&
		(from.center[0] + from.diameter-2) >= (to.center[0] - to.diameter-2)); // StartA <= EndB && EndA >=StartB
	bool doYOverlap = ((from.center[1] - from.diameter-2) <= (to.center[1] + to.diameter-2) &&
		(from.center[1] + from.diameter-2) >= (to.center[1] - to.diameter-2)); // StartA <= EndB && EndA >=StartB

	int xDir = (from.center[0] > to.center[0]) ? -1 : 1;
	int yDir = (from.center[1] > to.center[1]) ? -1 : 1;

	int xDistance = abs(from.center[0] - to.center[0]);
	int yDistance = abs(from.center[1] - to.center[1]);

	// We need to set up the correct y start in case x does overlap
	std::array<int, 2> corridorStart = {from.center[0], from.center[1]+(yDir*(from.diameter-1))};

	// Adding tiles until x-axis levels are identical
	if (!doXOverlap) {
		xDistance -= (from.diameter-1);
		corridorStart = { from.center[0] + (xDir * (from.diameter - 1)), from.center[1] };
		for (int i = 1; i <= xDistance; ++i) {
			corridorStart[0] += xDir * 1;
			room.floorPositions.push_back(corridorStart);
			// Sometimes, we just need to make sure a path exists - no need for aesthetics with walls
			if (hasWalls) {
				room.wallPositions.push_back({ corridorStart[0], corridorStart[1] + 1 });
				room.wallPositions.push_back({ corridorStart[0], corridorStart[1] - 1 });
			}
		}
	}
	// Same as above for y axis
	if (!doYOverlap) {
		yDistance -= (from.diameter - 1);
		for (int i = 1; i < yDistance; ++i) {
			corridorStart[1] += yDir * 1;
			room.floorPositions.push_back(corridorStart);
			if (hasWalls) {
				room.wallPositions.push_back({ corridorStart[0] + 1, corridorStart[1] });
				room.wallPositions.push_back({ corridorStart[0] - 1, corridorStart[1] });
			}
		}
	}
	// Yes, the player can know in which direction the exit room is by seeing which wall has 2 exits
	// Shut up
}

void RoomGenerator::createWall(Room& room, const std::array<int, 2>& from, const std::array<int, 2>& to, const bool& hasExit) {
	int wallLength = abs(from[0] - to[0]) + abs(from[1] - to[1]);
	int exitPoint = -1;
	if (hasExit) {
		exitPoint = getRandomNumber(1, wallLength - 1); // Place the exit at random location along the wall
	}
	if (abs(from[0] - to[0]) == 0) { // Wall is horizontal
		for (int i = 0; i <= wallLength; ++i) {
			if (i != exitPoint) {
				room.wallPositions.push_back(std::array<int, 2>({ from[0], from[1] + i }));
			}
		}
	}
	else { // Wall is vertical
		for (int i = 0; i <= wallLength; ++i) {
			if (i == exitPoint) {
				continue;
			}
			room.wallPositions.push_back(std::array<int, 2>({ from[0] + i, from[1] }));
		}
	}
}