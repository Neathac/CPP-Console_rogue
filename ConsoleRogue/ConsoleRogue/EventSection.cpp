#include "GameState.h"
#include "libtcod.hpp"
#include "SDL.h"
#include <string>
#include <deque>

void EventSection::colorArea(tcod::Console& console, tcod::ContextPtr& context) {
	tcod::draw_rect(console, { 0, PLAY_AREA_HEIGHT, EVENT_AREA_WIDTH, EVENT_AREA_HEIGHT }, ' ', std::nullopt, this->palette->eventBackground);
	context->present(console);
}

void EventSection::newEvent(tcod::Console& console, tcod::ContextPtr& context, std::string eventDescription) {
	this->events.push_back(eventDescription);
	while (this->events.size() > 5) { this->events.pop_front(); } // Sometimes, enemies may create more then 1 event per player action
	this->drawEvents(console, context);
}

void EventSection::drawEvents(tcod::Console& console, tcod::ContextPtr& context) {
	colorArea(console, context);
	for (int i = 0; i < this->events.size(); ++i) {
		tcod::print(console, { 1, PLAY_AREA_HEIGHT + 2 + (i*2) }, this->events[i], this->palette->eventHeaders, this->palette->eventBackground);
	}
}