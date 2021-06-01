#include "screen_lobby.h"

/*
* This method is called when the game is first created. It
* is used to load all assets and objects needed for the
* screen.
*/
void LobbyScreen::init() {
    // Request for data
    //app->getNetworkManager()->sendLobbyInfo(app->getLobbyId());
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
    }
}

/**
* This method is called whenever a packet is received from
* the app.
*/
void LobbyScreen::handlePacket(string packet) {
    size_t index;
    switch (packet.at(0)) {
    case 'I':
        cout << "Received info about lobby" << endl;
        index = packet.find_first_of('IT/');
        cout << index << endl;
        if (index != string::npos) {
            string num = packet.substr(index + 1);
            players = stoi(num);
        } else {
            players = 0;
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
    TexMan::drawImage("assets/register_bg.png", 0, 0, 900, 600);

    // Render the header rect
    TexMan::drawRect({ 17, 76, 122, 255 }, 0, 0, 900, 90);

    // Render the back button
    TexMan::drawHoverImage("assets/exit_btn.png", 20, 20, 100, 50, mx, my);

    // Render the number of players
    TexMan::drawText("Players: " + to_string(players), { 255, 255, 255, 255 }, 40, 450, 300);
}

/**
* This method is called whenever the screen needs to be cleaned
*/
void LobbyScreen::clean() {

}