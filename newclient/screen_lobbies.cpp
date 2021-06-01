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
        hasSelected = false;

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

    switch (packet.at(0)) {
    case 'L':
        // Clear lobbies
        lobbies.clear();
        start = packet.find_first_of("LT/");

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

            // Update selected too
            if (selected.id == lobby.id) {
                selected = lobby;
            }
        }
        cout << "Received " << lobbies.size() << " lobbies." << endl;
        break;
    case 'J':
        cout << "Joining lobby " << selected.id << "..." << endl;
        app->setLobbyId(selected.id);
        app->openScreen(3); // open lobby screen
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

        if (y > 900 || y < -200) {
            continue; // no point drawing these.
        }

        // Render the lobby bg
        TexMan::drawHoverImage("assets/lobby_btn.png", x, y, 200, 200, mx, my);

        // Draw the name
        TexMan::drawText(l.name, { 255, 255, 255, 255 }, 20, x + 100, y + 100);

        // Draw the capacity
        string s = to_string(l.size) + "/" + to_string(l.maxsize);
        TexMan::drawText(s, { 255, 255, 255, 255 }, 15, x + 175, y + 25);
    }

    // Render the header rect
    TexMan::drawRect({ 17, 76, 122, 255 }, 0, 0, 550, 90);

    // Render the back button
    TexMan::drawHoverImage("assets/back_btn.png", 20, 20, 100, 50, mx, my);

    // Render the custom button
    TexMan::drawHoverImage("assets/custom_btn.png", 430, 20, 100, 50, mx, my);

    // Selected
    if (hasSelected) {

        // Render confirmation box
        TexMan::drawRect({ 17, 76, 122, 255 }, 600, 150, 250, 300);

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