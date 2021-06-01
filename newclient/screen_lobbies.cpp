#include "screen_lobbies.h"
#include "netman.h"

/*
* This method is called when the game is first created. It
* is used to load all assets and objects needed for the
* screen.
*/
void LobbiesScreen::init() {
    // Send a list lobbies
    ticksSinceList = 0;
    app->getNetworkManager()->sendListLobbies();
}

/**
* This method is called whenever an event happens and this
* screen is currently being rendered.
*/
void LobbiesScreen::handleEvent(SDL_Event& event) {

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

            app->openScreen(0);
        }

        // custom 430 20 100 50
        if (x >= 430 && x <= 430 + 100 &&
            y >= 20 && y <= 20 + 50) {

            app->openScreen(0);
        }
    }

    if (event.type == SDL_MOUSEWHEEL) {
        
        int y = event.wheel.y;
        
        scrollVY += y * -20;
    }
}

/**
* This method is called whenever a packet is received from
* the app.
*/
void LobbiesScreen::handlePacket(string packet) {
    switch (packet.at(0)) {
    case 'L':
        // Clear lobbies
        lobbies.clear();
        size_t start = packet.find_first_of("LT/");

        // Bad input
        if (start == string::npos) {
            return;
        }

        // Cut out the LT/
        packet = packet.substr(start + 3);

        // Maximum of 100 lobbies
        size_t index;
        for (int i = 0; i < 100; i++) {
            // Parse ID
            index = packet.find_first_of('/');
            if (index == string::npos) break;
            int id = stoi(packet.substr(0, index));
            packet = packet.substr(index + 1);

            // Parse name
            index = packet.find_first_of('/');
            if (index == string::npos) break;
            string name = packet.substr(0, index);
            packet = packet.substr(index + 1);

            // Parse size
            index = packet.find_first_of('/');
            if (index == string::npos) break;
            int size = stoi(packet.substr(0, index));
            packet = packet.substr(index + 1);

            // Parse maxsize
            index = packet.find_first_of('\n');
            if (index == string::npos) break;
            int maxsize = stoi(packet.substr(0, index));
            packet = packet.substr(index + 1);

            // Create lobby
            Lobby lobby = { id, name, size, maxsize };
            lobbies.push_back(lobby);
        }
            
        
        break;
    }
}

/**
* If there is any kind of logic that needs to run per tick,
* this is the place to do it. This function fires 30 times
* a second (once per frame) when the screen is active
*/
void LobbiesScreen::update() {

    int height = (int)(lobbies.size() / 2) * 250 + 370;
    int maxScrollY = height - 600;

    if (maxScrollY > 0) {
        scrollVY *= 0.75;
        scrollY += round(scrollVY);

        if (scrollY > maxScrollY) {
            scrollY = maxScrollY;
        }
        if (scrollY < 0) {
            scrollY = 0;
        }
    }

    // Periodically fetch new list
    ticksSinceList++;
    if (ticksSinceList > 300) { // 5s
        app->getNetworkManager()->sendListLobbies();
        ticksSinceList = 0;
    }
}

/**
* This method is called whenever the screen needs to be
* rendered. All pre and post steps are done by the app, so
* this function only needs to render its components
*/
void LobbiesScreen::render() {

    // Render the background
    TexMan::drawImage("assets/register_bg.png", 0, 0, 900, 600);

    // Render the lobby list background
    TexMan::drawRect({ 17, 76, 122, 255 }, 0, 0, 550, 600);

    // Render each lobby
    for (int i = 0; i < lobbies.size(); i++) {
        Lobby l = lobbies[i];

        // Top left corner of lobby
        int x = 50 + 250 * (i % 2);
        int y = 120 + 250 * (int)(i / 2) - scrollY;

        // Render the lobby bg
        TexMan::drawImage("assets/lobby_btn.png", x, y, 200, 200);

        // Draw the name
        TexMan::drawText(l.name, { 255, 255, 255, 255 }, 20, x + 100, y + 100);

        // Draw the capacity
        string s = to_string(l.size) + "/" + to_string(l.maxsize);
        TexMan::drawText(s, { 255, 255, 255, 255 }, 15, x + 175, y + 25);
    }

    // Render the header rect
    TexMan::drawRect({ 17, 76, 122, 255 }, 0, 0, 550, 90);

    // Render the back button
    TexMan::drawImage("assets/back_btn.png", 20, 20, 100, 50);

    // Render the custom button
    TexMan::drawImage("assets/custom_btn.png", 430, 20, 100, 50);
}

/**
* This method is called whenever the screen needs to be cleaned
*/
void LobbiesScreen::clean() {
    lobbies.clear();
}