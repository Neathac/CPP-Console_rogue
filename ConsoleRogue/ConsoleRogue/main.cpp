#include "libtcod.hpp"
#include "SDL.h"
#include "GameState.h"
#include <iostream>
// SDL defines main and causes errors
#undef main

int main() {
    Palette* palette = new Palette();
    Game* gameState = new Game(std::make_shared<Palette> (*palette));
    while (1) {  // Game loop.
        // TCOD_console_clear(console.get());
        SDL_Event event;
        SDL_WaitEvent(nullptr);  // Optional, sleep until events are available.
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                return 0;  // Exit.
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                case SDLK_RIGHT:
                    gameState->playerMove(DIRECTIONS::MOVE_RIGHT);
                    break;
                case SDLK_UP:
                    gameState->playerMove(DIRECTIONS::MOVE_UP);
                    break;
                case SDLK_LEFT:
                    gameState->playerMove(DIRECTIONS::MOVE_LEFT);
                    break;
                case SDLK_DOWN:
                    gameState->playerMove(DIRECTIONS::MOVE_DOWN);
                    break;
                case SDLK_SPACE:
                    gameState->playerInterract();
                    break;
                }
                
            }
        }
    }
}