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
    player.id = -1;
    dead = false;
    gameover = false;
}

int dir = -1;

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

        if (dir != 2 && dir != 0 && (key == "W" || key == "Up")) {
            app->getNetworkManager()->sendPlayerDir(0);
            dir = 0;
        }

        if (dir != 3 && dir != 1 && (key == "D" || key == "Right")) {
            app->getNetworkManager()->sendPlayerDir(1);
            dir = 1;
        }

        if (dir != 0 && dir != 2 && (key == "S" || key == "Down")) {
            app->getNetworkManager()->sendPlayerDir(2);
            dir = 2;
        }

        if (dir != 1 && dir != 3 && (key == "A" || key == "Left")) {
            app->getNetworkManager()->sendPlayerDir(3);
            dir = 3;
        }
    }
}

/**
* This method is called whenever a packet is received from
* the app.
*/
void GameScreen::handlePacket(string packet) {

    // split packet into entries
    vector<string> entries = NetMan::split(packet, "\n");
    vector<string> parts;
    string type;

    for (string entry : entries) {

        // split entry into parts
        parts = NetMan::split(entry, "/");
        type = parts[0];

        if (type == "ET") {
            cout << "Successfully left lobby" << endl;
            app->openScreen(1);
        }
        if (type == "EF") {
            cout << "Failed to leave lobby" << endl;
            exiting = false;
        }
        if (type == "W") {
            gameover = true;
            winner = parts[1];
            ticksSinceWin = 0;
        }
        if (type == "M") {
            // Parse key data
            int id = stoi(parts[1]);
            int x = stoi(parts[2]);
            int y = stoi(parts[3]);

            // Store player
            Player p = {id , x, y };
            if (id == app->getPlayerId()) {
                // Update our player
                if (player.id != -1) {
                    players.push_back(player);
                }
                player = p;

                // For each other player
                for (int i = 0; i < players.size(); i++) {

                    Player o = players[i];

                    // Did I collide?
                    if (!dead && o.x == p.x && o.y == p.y) {

                        // send dead event
                        app->getNetworkManager()->sendPlayerDead();
                        dead = true;
                    }
                }
            }
            else {
                // It's not, store them on map
                players.push_back(p);
            }

            // Do not have color for this guy
            if (colors.find(id) == colors.end()) {
                colors.insert(make_pair(id, randomColor()));
            }
        }
    }
}

/**
* If there is any kind of logic that needs to run per tick,
* this is the place to do it. This function fires 30 times
* a second (once per frame) when the screen is active
*/
void GameScreen::update() {
    if (gameover && !exiting) {
        ticksSinceWin++;
        if (ticksSinceWin > 120) {
            app->getNetworkManager()->sendExitLobby();
            exiting = true;
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

    if (gameover) {
        // Draw game over text
        TexMan::drawText("Winner: " + winner, { 255, 255, 255, 255 }, 50, 450, 300);
    } else if (!dead) {
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