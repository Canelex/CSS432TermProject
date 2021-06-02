#include "screen_game.h"

SDL_Color randomColor() {
    Uint8 r = rand() % 256;
    Uint8 g = rand() % 256;
    Uint8 b = rand() % 256;
    return { r, g, b, 255 };
}

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
        index = packet.find_first_of("M/");

        // Bad input
        if (index == string::npos) {
            return;
        }

        // Cut out the M/
        packet = packet.substr(index + 2);

        // Maximum of 100 players
        for (int i = 0; i < 100; i++) {
            // Parse ID
            index = packet.find_first_of('/');
            if (index == string::npos) break;
            int id = stoi(packet.substr(0, index));
            packet = packet.substr(index + 1);

            // Parse x
            index = packet.find_first_of('/');
            if (index == string::npos) break;
            int x = stoi(packet.substr(0, index));
            packet = packet.substr(index + 1);

            // Parse y
            index = packet.find_first_of('\n');
            if (index == string::npos) break;
            int y = stoi(packet.substr(0, index));
            packet = packet.substr(index + 1);

            // Store player
            Player p = { id, x, y };
            if (id == app->getPlayerId()) {
                players.push_back(player);
                player = p;
                // This is our player!
            } else {
                // It's not, store them on map
                players.push_back(p);
            }

            // Do not have color for this guy
            if (colors.find(id) == colors.end()) {
                colors.insert(make_pair(id, randomColor()));
            }
        }
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
        if (!dead && player.x == p.x && player.y == p.y) {

            // send dead event
            app->getNetworkManager()->sendPlayerDead();
            dead = true;
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

    // Draw each player segment
    for (int i = 0; i < players.size(); i++) {

        // Render each player square
        Player p = players[i];

        SDL_Color color = colors.find(p.id)->second;

        // Render each rect
        TexMan::drawRect(color, mx + p.x * 10, my + p.y * 10, 10, 10);
    }

    // Draw each color
    map<int, SDL_Color>::iterator it;
    int y = 50;
    for (it = colors.begin(); it != colors.end(); ++it) {
        SDL_Color c = it->second;
        int id = it->first;

        TexMan::drawText(to_string(id), c, 20, 50, y);
        y += 20;
    }
    
    if (!dead) {
        // Draw your player
        TexMan::drawRect({ 255, 255, 255, 255 }, mx + player.x * 10, my + player.y * 10, 10, 10);
    } else {
        // Draw game over text
        TexMan::drawText("Game Over.", { 255, 255, 255, 255 }, 30, 450, 30);
    }
}

/**
* This method is called whenever the screen needs to be cleaned
*/
void GameScreen::clean() {

}