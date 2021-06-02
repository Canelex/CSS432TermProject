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
    hasSelected = false;
    joining = false;
    scrollY = 0;
    scrollVY = 0;
    app->getNetworkManager()->sendListLobbies();
}

/**
* This method is called whenever an event happens and this
* screen is currently being rendered.
*/
void LobbiesScreen::handleEvent(SDL_Event& event) {
    if (event.type == SDL_MOUSEMOTION) {
        mx = event.button.x;
        my = event.button.y;
    }

    if (joining) {
        return; // lock screen when loading.
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

        // Did we click confirm? 625, 375, 200, 50
        if (x >= 625 && x <= 625 + 200 &&
            y >= 375 && y <= 375 + 50) {

            app->getNetworkManager()->sendJoinLobby(selected.id);
            joining = true;
            return;
        }

        // back 20 20 100 50
        if (x >= 20 && x <= 20 + 100 &&
            y >= 20 && y <= 20 + 50) {

            app->openScreen(0);
        }

        // custom 430 20 100 50
        if (x >= 430 && x <= 430 + 100 &&
            y >= 20 && y <= 20 + 50) {

            app->openScreen(2);
        }

        // Each lobby
        for (int i = 0; i < lobbies.size(); i++) {
            // Top left corner of lobby
            Lobby l = lobbies[i];
            int lx = 50 + 250 * (i % 2);
            int ly = 120 + 250 * (int)(i / 2) - scrollY;

            // is it on lobby?
            if (x >= lx && x <= lx + 200 && y >= ly && y <= ly + 200) {
                // Show lobby info
                selected = l;
                hasSelected = true;
                // Get some more info though too
                app->getNetworkManager()->sendListLobbies();
            }
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
    size_t start;
    Lobby lobby;
    vector<string> entries;
    vector<string> parts;

    switch (packet.at(0)) {
    case 'L':
        // Clear lobbies
        lobbies.clear();
        
        // Split packet into entries
        entries = NetMan::split(packet, "\n");
        for (string entry : entries) {
            // Split entry into parts
            parts = NetMan::split(entry, "/");

            cout << "Lobby List Entry: " << entry << endl;

            lobby = {};
            if (parts.size() == 6) { 
                // Has LT/
                lobby.id = stoi(parts[1]);
                lobby.name = parts[2];
                lobby.size = stoi(parts[3]);
                lobby.maxsize = stoi(parts[4]);
                lobby.playing = stoi(parts[5]);
                lobby.pinned = lobby.id <= 3 ? true : false;
                lobbies.push_back(lobby);
            }
            else if (parts.size() == 5) {
                // Regular ol' entry
                lobby.id = stoi(parts[0]);
                lobby.name = parts[1];
                lobby.size = stoi(parts[2]);
                lobby.maxsize = stoi(parts[3]);
                lobby.playing = stoi(parts[4]);
                lobby.pinned = lobby.id <= 3 ? true : false;
                lobbies.push_back(lobby);
            }
            else {
                cout << "Received malformed lobby info packet." << endl;
            }
        }
        break;
    case 'J':
        if (packet == "JT\n") {
            cout << "Joining lobby " << selected.id << "..." << endl;
            app->setLobby(selected.id, selected.name, selected.maxsize);
            app->openScreen(3); // open lobby screen
        }
        else {
            cout << "Failed to join lobby" << endl;
        }
        joining = false;
        break;
    }
}

/**
* If there is any kind of logic that needs to run per tick,
* this is the place to do it. This function fires 30 times
* a second (once per frame) when the screen is active
*/
void LobbiesScreen::update() {

    int height = (int)ceil(lobbies.size() * 0.5) * 250 + 120;
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
    TexMan::drawImage("assets/bg.png", 0, 0, 900, 600);

    // Render the lobby list background
    TexMan::drawRect({ 0, 0, 0, 180 }, 0, 0, 550, 600);

    // Render each lobby
    TexMan::clip(true, 0, 90, 550, 600-90);
    for (int i = 0; i < lobbies.size(); i++) {
        Lobby l = lobbies[i];

        // Top left corner of lobby
        int x = 50 + 250 * (i % 2);
        int y = 120 + 250 * (int)(i / 2) - scrollY;

        if (y > 900 || y < -200) {
            continue; // no point drawing these.
        }

        if (selected.id == l.id) {
            TexMan::drawRect({ 255, 255, 255, 255 }, x - 2, y - 2, 200 + 4, 200 + 4);
        }

        // Render the lobby bg
        TexMan::drawHoverImage("assets/lobby_btn.png", x, y, 200, 200, mx, my);

        // Draw the name
        TexMan::drawText(l.name, { 255, 255, 255, 255 }, 20, x + 100, y + 100);
        if (l.playing) {
            TexMan::drawText("(Playing)", { 255, 255, 255, 255 }, 15, x + 100, y + 120);
        } else if (l.size == l.maxsize) {
            TexMan::drawText("(Full)", { 255, 255, 255, 255 }, 15, x + 100, y + 120);
        }

        // Draw the capacity
        string s = to_string(l.size) + "/" + to_string(l.maxsize);
        TexMan::drawText(s, { 255, 255, 255, 255 }, 15, x + 175, y + 25);

        // Draw pin icon if needed
        if (l.pinned) {
            TexMan::drawImage("assets/pin.png", x + 10, y + 15, 25, 25);
        }
    }
    TexMan::clip(false, 0, 0, 0, 0);

    // Render the back button
    TexMan::drawHoverImage("assets/back_btn.png", 20, 20, 100, 50, mx, my);

    // Render the custom button
    TexMan::drawHoverImage("assets/custom_btn.png", 430, 20, 100, 50, mx, my);

    // Selected
    if (hasSelected) {

        // Render confirmation box
        TexMan::drawRect({ 0, 0, 0, 180 }, 600, 150, 250, 300);

        // Render lobby name
        TexMan::drawImage("assets/register_textbox.png", 625, 175, 200, 50);
        TexMan::drawText(selected.name, { 255, 255, 255, 255 }, 20, 725, 200);

        // Draw the capacity
        string s = to_string(selected.size) + "/" + to_string(selected.maxsize);
        TexMan::drawText(s, { 255, 255, 255, 255 }, 40, 725, 300);


        // Draw the join button
        TexMan::drawHoverImage("assets/register_btn.png", 625, 375, 200, 50, mx, my);
    }
}

/**
* This method is called whenever the screen needs to be cleaned
*/
void LobbiesScreen::clean() {
    lobbies.clear();
}