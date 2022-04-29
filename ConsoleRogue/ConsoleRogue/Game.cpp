#include "GameState.h"

void Game::playerMove(DIRECTIONS direction) {
	switch (direction) {
	case DIRECTIONS::MOVE_DOWN:
		this->player.placeSelf(this->playArea, this->player.position[0], this->player.position[1] + 1);
		break;
	case DIRECTIONS::MOVE_UP:
		this->player.placeSelf(this->playArea, this->player.position[0], this->player.position[1] - 1);
		break;
	case DIRECTIONS::MOVE_LEFT:
		this->player.placeSelf(this->playArea, this->player.position[0] - 1, this->player.position[1]);
		break;
	case DIRECTIONS::MOVE_RIGHT:
		this->player.placeSelf(this->playArea, this->player.position[0] + 1, this->player.position[1]);		
		break;
	}
	this->playArea.drawWholeMap(this->console, this->context);
}