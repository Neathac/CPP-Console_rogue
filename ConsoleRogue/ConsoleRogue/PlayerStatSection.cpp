#include "GameState.h"
#include "libtcod.hpp"
#include "SDL.h"
#include <string>

void PlayerStatSection::colorArea(tcod::Console& console, tcod::ContextPtr& context) {
	tcod::draw_rect(console, { PLAY_AREA_WIDTH, 0, STAT_AREA_WIDTH, STAT_AREA_HEIGHT }, ' ', std::nullopt, this->palette->statBackground);
	context->present(console);
}

void PlayerStatSection::drawTextFields(tcod::Console& console, tcod::ContextPtr& context) {
	tcod::print(console, { PLAY_AREA_WIDTH + 1,  2 }, "PLAYER STATS", this->palette->statHeaders, this->palette->statBackground);
	tcod::print(console, { PLAY_AREA_WIDTH + 1,  6 }, "Health: ", this->palette->statHeaders, this->palette->statBackground);
	tcod::print(console, { PLAY_AREA_WIDTH + 1,  8 }, "Damage: ", this->palette->statHeaders, this->palette->statBackground);
	tcod::print(console, { PLAY_AREA_WIDTH + 1,  10 }, " Speed: ", this->palette->statHeaders, this->palette->statBackground);
	tcod::print(console, { PLAY_AREA_WIDTH + 1,  12 }, " Armor: ", this->palette->statHeaders, this->palette->statBackground);
	tcod::print(console, { PLAY_AREA_WIDTH + 1,  14 }, " Range: ", this->palette->statHeaders, this->palette->statBackground);
	context->present(console);
}

void PlayerStatSection::drawStatValues(tcod::Console& console, tcod::ContextPtr& context) {
	tcod::print(console, { PLAY_AREA_WIDTH + 9,  6 }, std::to_string(this->player->health) + "/" + std::to_string(this->player->maxHealth), this->palette->statHeaders, this->palette->statBackground);
	tcod::print(console, { PLAY_AREA_WIDTH + 9,  8 }, std::to_string(this->player->damage), this->palette->statHeaders, this->palette->statBackground);
	tcod::print(console, { PLAY_AREA_WIDTH + 9,  10 }, std::to_string(this->player->speed), this->palette->statHeaders, this->palette->statBackground);
	tcod::print(console, { PLAY_AREA_WIDTH + 9,  12 }, std::to_string(this->player->armor), this->palette->statHeaders, this->palette->statBackground);
	tcod::print(console, { PLAY_AREA_WIDTH + 9,  14 }, std::to_string(this->player->range), this->palette->statHeaders, this->palette->statBackground);
	context->present(console);
}