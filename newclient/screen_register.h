#pragma once
#include "app.h"
#include <string>

class RegisterScreen : public Screen
{
public:
    /**
    * This constructor just stores a reference to the renderer
    * for later (rendering).
    */
    RegisterScreen(App* app, SDL_Renderer* renderer) : Screen(app, renderer) {}

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
    * This method is called whenever a packet is received from
    * the app.
    */
    void handlePacket(string packet);

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

    /**
    * This method is called whenever the screen needs to be cleaned
    */
    void clean();

private:
    // Background
    SDL_Rect rectBG;
    SDL_Texture* texBG;
    std::string username;

    // Username Rect
    SDL_Rect rectName;
    SDL_Texture* texName;

    // Button rect + image
    SDL_Rect rectBtn;
    SDL_Texture* texBtn;
};

