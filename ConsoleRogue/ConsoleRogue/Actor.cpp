#include "GameState.h"

Actor::Actor(ACTOR_TYPE type, std::array<int, 2> position) : type(type) {
	this->position[0] = position[0];
	this->position[1] = position[1];
	status = Tileset::floor;
	health = 5;
	maxHealth = 5;
	speed = 0;
	damage = 5;
	armor = 5;
	range = 2;
	speedLimit = 100;
	type = ACTOR_TYPE::_count; // Just for initialization
}