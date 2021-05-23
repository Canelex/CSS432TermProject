/**
 * game.h/cpp
 * 5/23/2021
 *
 * This class is responsible for all things related to the game itself,
 * including input, rendering, and updating.It uses the class ____ for
 * networking and connecting to the server.
 */
#pragma once

#include <iostream>
#include "packages/sdl2.2.0.5/build/native/include/SDL.h"
using namespace std;

class Game
{
public:
    /* Constructor and destructor */
    Game();
    ~Game();

    /* Initialization function for starting the game */
    void init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);

    /* Functions that will be called in the game loop */
    void handleEvents();
    void update();
    void render();
    void clean();

    /* Getters and setters */
    bool running() const;
private:
    /* Is the game running? */
    bool isRunning;
    SDL_Window* window;
    SDL_Renderer* renderer;
};

