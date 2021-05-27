/**
* screen.h/cpp
* 5/24/2021
* 
* This abstract class is extended and implemented by the different
* screens. It provides the interface for handling screen input,
* rendering, and updating. If the screen is not being actively
* shown, it will not receive any calls.
*/

#pragma once
#include "app.h"
#include "SDL.h"
#include "SDL_image.h"

// Define app ahead of time
class App;

class Screen
{
public:
    /**
    * This constructor just stores a reference to the renderer
    * for later (rendering).
    */
    Screen(App* app, SDL_Renderer* renderer) {
        this->app = app;
        this->renderer = renderer;
    }

    /*
    * This method is called when the game is first created. It
    * is used to load all assets and objects needed for the
    * screen.
    */
    virtual void init() = 0;

    /**
    * This method is called whenever an event happens and this
    * screen is currently being rendered.
    */
    virtual void handleEvent(SDL_Event& event) = 0;

    /**
    * If there is any kind of logic that needs to run per tick,
    * this is the place to do it. This function fires 30 times
    * a second (once per frame) when the screen is active
    */
    virtual void update() = 0;

    /**
    * This method is called whenever the screen needs to be
    * rendered. All pre and post steps are done by the app, so
    * this function only needs to render its components
    */
    virtual void render() = 0;
protected:
    // Reference to app object
    App* app;

    // Reference to renderer object
    SDL_Renderer* renderer;
};

