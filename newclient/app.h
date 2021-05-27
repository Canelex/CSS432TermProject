/**
* app.h/cpp
* 5/23/2021
*
* This class is responsible for all things related to the application and
* game. This includes including input, rendering, and updating for screens
* such as register, game selection, and playing. It uses the class ____ for
* networking and connecting to the server.
*/

#pragma once
#include <iostream>
#include <vector>
#include <SDL.h>
#include "screen.h"
using namespace std;

// Define screen ahead of time
class Screen;

class App
{
public:
    /* Constructor and destructor */
    App();
    ~App();

    /* Initialization function for starting the game */
    void init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);

    /**
    * This function handles all user input to the system. It is called every
    * tick by the main application loop.
    */
    void handleEvents();

    /**
    * This function handles all the game logic. It is called every tick by
    * the main application loop.
    */
    void update();

    /**
    * This function is responsible for rendering to the screen. It is called
    * every tick by the main loop, after inputs & updates have been ran.
    */
    void render();

    /**
    * This function is responsible for cleaning up the application. It is called
    * by the main process after the game has completed.
    */
    void clean();

    /**
    * This function opens another screen. It accepts an id parameter which switches
    * the activeScreenIndex field
    */
    void openScreen(int id);

    /**
    * Returns whether the application is still running.
    */
    bool isRunning() const;

private:
    // Is the game running?
    bool running;

    // vector of screens
    vector<Screen*> screens;

    // Which screen is active?
    int activeScreenIndex;

    // SDL window pointer
    SDL_Window* window;

    // SDL renderer pointer
    SDL_Renderer* renderer;
};

