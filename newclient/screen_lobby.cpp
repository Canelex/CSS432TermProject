#include "screen_lobby.h"

/*
* This method is called when the game is first created. It
* is used to load all assets and objects needed for the
* screen.
*/
void LobbyScreen::init() {
    // Request for data
    exiting = false;
}

/**
* This method is called whenever an event happens and this
* screen is currently being rendered.
*/
void LobbyScreen::handleEvent(SDL_Event& event) {
    if (event.type == SDL_MOUSEMOTION) {
        mx = event.button.x;
        my = event.button.y;
    }

    if (exiting) {
        return; // don't let them do anything else
    }

    // Mouse click
    if (event.type == SDL_MOUSEBUTTONDOWN) {

        // We only care about left clicks
        if (event.button.button != SDL_BUTTON_LEFT) {
            return;
        }

        // Was it in the back button?
        int x = event.button.x;
        int y = event.button.y;

        // back 20 20 100 50
        if (x >= 20 && x <= 20 + 100 &&
            y >= 20 && y <= 20 + 50) {

            exiting = true;
            app->getNetworkManager()->sendExitLobby();
        }

        // start 780 20 100 50
        if (players.size() >= 0 &&
            x >= 780 && x <= 780 + 100 &&
            y >= 20 && y <= 20 + 50) {

            // todo send host start packet
            exiting = true;
            app->getNetworkManager()->sendStartGame(app->getLobbyId());
        }
    }
}

/**
* This method is called whenever a packet is received from
* the app.
*/
void LobbyScreen::handlePacket(string packet) {

    vector<string> entries;
    vector<string> parts;
    size_t index;
    switch (packet.at(0)) {
    case 'I':
        cout << "Received info about lobby" << endl;
        players.clear();

        // Split packet into entries
        entries = NetMan::split(packet, "\n");
        for (string entry : entries) {
            // Split entry into part
            parts = NetMan::split(entry, "/");
            cout << "Lobby Info Entry: " << entry << endl;
            
            if (parts.size() == 3) {
                // Has IT/
                players.push_back(parts[2]);
            } else if (parts.size() == 2) {
                // Regular ol' entry
                players.push_back(parts[1]);
            } else {
                cout << "Received malformed lobby info packet." << endl;
            }
        }

        break;
    case 'E':
        if (packet == "ET\n") {
            cout << "Successfully left lobby" << endl;
            app->openScreen(1);
        } else {
            cout << "Failed to leave lobby" << endl;
            exiting = false;
        }
        break;
    case 'S':
        if (packet == "ST\n") {
            cout << "OPENING SCREEN 4" << endl;
            app->openScreen(4);
            cout << "Starting game" << endl;
        } else {
            cout << "Failed to start game" << endl;
        }
    }
}

/**
* If there is any kind of logic that needs to run per tick,
* this is the place to do it. This function fires 30 times
* a second (once per frame) when the screen is active
*/
void LobbyScreen::update() {

}

/**
* This method is called whenever the screen needs to be
* rendered. All pre and post steps are done by the app, so
* this function only needs to render its components
*/
void LobbyScreen::render() {

    // Render the background
    TexMan::drawImage("assets/bg.png", 0, 0, 900, 600);

    // Render the header rect
    TexMan::drawRect({ 0, 0, 0, 180}, 0, 0, 900, 90);

    // Render the back button
    TexMan::drawHoverImage("assets/exit_btn.png", 20, 20, 100, 50, mx, my);

    // Render the start button
    TexMan::drawHoverImage("assets/start_btn.png", 780, 20, 100, 50, mx, my);

    // Render the player list background
    TexMan::drawRect({ 0, 0, 0, 180}, 300, 150, 300, 400);
    // Render the title
    TexMan::drawText(app->getLobbyName(), { 255, 255, 255, 255 }, 35, 450, 200);

    // Render the number of players
    for (int i = 0; i < app->getLobbyMaxSize(); i++) {
        
        if (i < players.size()) {
            TexMan::drawText(players[i], { 255, 255, 255, 255 }, 20, 450, 250 + i * 25);
        } else {
            TexMan::drawText("[Empty]", { 200, 200, 200, 255 }, 20, 450, 250 + i * 25);
        }
    }
}

/**
* This method is called whenever the screen needs to be cleaned
*/
void LobbyScreen::clean() {

}