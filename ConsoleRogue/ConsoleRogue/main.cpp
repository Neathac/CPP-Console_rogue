#include "libtcod.hpp"
#include "SDL.h"
#include "GameState.h"
// SDL defines main and causes errors
#undef main

int main() {
    Game gamestate = Game();
    while (1) {  // Game loop.
        // TCOD_console_clear(console.get());
        SDL_Event event;
        SDL_WaitEvent(nullptr);  // Optional, sleep until events are available.
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                return 0;  // Exit.
            case SDL_KEYDOWN:
            //   tcod::print(console, { 7, 7 }, "@", tcod::ColorRGB(20, 48, 200), std::nullopt);
            //   tcod::print(console, { 1,0 }, "An up arrow", std::nullopt, std::nullopt);
            //   context->present(console);
                break;
            case SDLK_a:
            //    tcod::print(console, { 2,0 }, "A left arrow", std::nullopt, std::nullopt);
            //    context->present(console);
                break;
            }
        }
    }
}