#pragma once
#include "app.h"
#include "screen.h"
#include <string>
#include <vector>

struct Player {
    SDL_Color color;
    int x;
    int y;
    bool alive;
};

class GameScreen : public Screen
{
public:
    /**
    * This constructor just stores a reference to the renderer
    * for later (rendering).
    */
    GameScreen(App* app) : Screen(app) {}

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
    Player player;

    vector<Player> players;
    
    bool exiting;
    bool dead;
};

