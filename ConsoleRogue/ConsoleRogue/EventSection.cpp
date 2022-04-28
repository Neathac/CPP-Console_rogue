#include "GameState.h"
#include "libtcod.hpp"
#include "SDL.h"
#include <string>
#include <queue>

void EventSection::colorArea(tcod::Console& console, tcod::ContextPtr& context) {
	tcod::draw_rect(console, { 0, PLAY_AREA_HEIGHT, EVENT_AREA_WIDTH, EVENT_AREA_HEIGHT }, ' ', std::nullopt, this->palette->eventBackground);
	context->present(console);
}