#include "screen_game.h"


/*
* This method is called when the game is first created. It
* is used to load all assets and objects needed for the
* screen.
*/
void GameScreen::init() {
    exiting = false;
    players.clear();
    
    player.x = 100;
    player.y = 100;
    dead = false;
}

/**
* This method is called whenever an event happens and this
* screen is currently being rendered.
*/
void GameScreen::handleEvent(SDL_Event& event) {
    if (event.type == SDL_MOUSEMOTION) {
        mx = event.button.x;
        my = event.button.y;
    }

    if (exiting) {
        return; // don't let them do anything else
    }

    // Mouse click
    if (event.type == SDL_KEYDOWN) {
        
        string key = SDL_GetKeyName(event.key.keysym.sym);

        if (key == "Escape") {
            app->getNetworkManager()->sendExitLobby();
            exiting = true;
        }

        if (dead) {
            return; // no controls for dead people
        }

        if (key == "W" || key == "Up") {
            app->getNetworkManager()->sendPlayerDir(0);
        }

        if (key == "D" || key == "Right") {
            app->getNetworkManager()->sendPlayerDir(1);
        }

        if (key == "S" || key == "Down") {
            app->getNetworkManager()->sendPlayerDir(2);
        }

        if (key == "A" || key == "Left") {
            app->getNetworkManager()->sendPlayerDir(3);
        }
    }
}

/**
* This method is called whenever a packet is received from
* the app.
*/
void GameScreen::handlePacket(string packet) {
    size_t index;
    switch (packet.at(0)) {
    case 'E':
        if (packet == "ET\n") {
            cout << "Successfully left lobby" << endl;
            app->openScreen(1);
        }
        else {
            cout << "Failed to leave lobby" << endl;
            exiting = false;
        }
        break;
    case 'M':
        // Parse each player
        break;
    }
}

/**
* If there is any kind of logic that needs to run per tick,
* this is the place to do it. This function fires 30 times
* a second (once per frame) when the screen is active
*/
void GameScreen::update() {
    // Draw each player
    for (int i = 0; i < players.size(); i++) {

        // Render each player square
        Player p = players[i];

        // Did I collide?
        if (player.x + 10 >= p.x && player.x <= p.x + 10 &&
            player.y + 10 >= p.y && player.y <= p.y + 10) {

            // send dead event
            app->getNetworkManager()->sendPlayerDead();

        }
    }

}

/**
* This method is called whenever the screen needs to be
* rendered. All pre and post steps are done by the app, so
* this function only needs to render its components
*/
void GameScreen::render() {

    // Render the background
    TexMan::drawImage("assets/bg.png", 0, 0, 900, 600);

    // Render the game
    int mx = 200;
    int my = 50;
    int ms = 10;
    TexMan::drawRect({ 0, 0, 0, 180 }, mx, my, 50 * ms, 50 * ms);

    // Draw each player
    for (int i = 0; i < players.size(); i++) {

        // Render each player square
        Player p = players[i];

        // Render each rect
        TexMan::drawRect(p.color, mx + p.x, my + p.y, 10, 10);
    }

    // Draw your player
    TexMan::drawRect({ 255, 255, 255, 255 }, mx + player.x, my + player.y, 10, 10);
}

/**
* This method is called whenever the screen needs to be cleaned
*/
void GameScreen::clean() {

}