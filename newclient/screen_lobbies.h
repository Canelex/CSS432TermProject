#pragma once
#include "screen.h"

class LobbiesScreen : public Screen
{
public:
    /**
    * This constructor just stores a reference to the renderer
    * for later (rendering).
    */
    LobbiesScreen(App* app, SDL_Renderer* renderer) : Screen(app, renderer) {}

    /*
    * This method is called when the game is first created. It
    * is used to load all assets and objects needed for the
    * screen.
    */
    void init();

    /**
    * This method is called whenever an event happens and this
    * screen is currently being rendered.
    */
    void handleEvent(SDL_Event& event);

    /**
    * If there is any kind of logic that needs to run per tick,
    * this is the place to do it. This function fires 30 times
    * a second (once per frame) when the screen is active
    */
    void update();

    /**
    * This method is called whenever the screen needs to be
    * rendered. All pre and post steps are done by the app, so
    * this function only needs to render its components
    */
    void render();

private:
    // Background objects
    SDL_Rect rectBG;
    SDL_Texture* texBG;
};
