#include "GameState.h"
#include "libtcod.hpp"
#include "SDL.h"

void PlayerStatSection::colorArea(tcod::Console& console, tcod::ContextPtr& context) {
	tcod::draw_rect(console, { PLAY_AREA_WIDTH, 0, STAT_AREA_WIDTH, STAT_AREA_HEIGHT }, ' ', std::nullopt, this->palette->statBackground);
}