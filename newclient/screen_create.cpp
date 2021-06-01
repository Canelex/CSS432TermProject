#include "screen_create.h"

/*
* This method is called when the game is first created. It
* is used to load all assets and objects needed for the
* screen.
*/
void CreateScreen::init() {
    name = "";
    text = "";
    joining = false;
}

/**
* This method is called whenever an event happens and this
* screen is currently being rendered.
*/
void CreateScreen::handleEvent(SDL_Event& event) {
    if (event.type == SDL_MOUSEMOTION) {
        mx = event.button.x;
        my = event.button.y;
    }

    if (joining) {
        return;
    }

    // Key press
    if (event.type == SDL_KEYDOWN) {
        string key = SDL_GetKeyName(event.key.keysym.sym);

        // Erase key
        if (key == "Backspace" && name.size() > 0) {
            name = name.substr(0, name.size() - 1);
        }

        // Name is too long
        if (name.size() >= 16) {
            return;
        }

        if (key == "Space") {
            name += " ";
        }

        // Key is too long
        if (key.size() > 1) {
            return;
        }

        // Valid key
        if ((key >= "A" && key <= "Z") || (key >= "0" && key <= "9")) {
            name += key;
        }
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

            app->openScreen(1);
        }

        // check collisions
        for (int i = 0; i < 5; i++) {

            int bx = 350 + i * 42;
            int by = 283;

            if (x >= bx && x <= bx + 32 && y >= by && y <= by + 32) {
                size = sizes[i];
                return;
            }
        }

        // In the box
        if (name.size() >= 3 && name.size() <= 16 && x >= 350 && x <= 350 + 200 && y >= 335 && y <= 335 + 50) {
            app->getNetworkManager()->sendCreateLobby(name, size);
            joining = true;
            return;
        }
       
    }
}

/**
* This method is called whenever a packet is received from
* the app.
*/
void CreateScreen::handlePacket(string packet) {
    size_t index;
    switch (packet.at(0)) {
    case 'C':
        cout << "Created a lobby" << endl;
        index = packet.find_first_of('CT/');
        cout << index << endl;
        if (index != string::npos) {
            string num = packet.substr(index + 1);
            int lobbyId = stoi(num);
            app->setLobbyId(lobbyId);
            app->openScreen(3);
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
void CreateScreen::update() {
    // Update text
    text = name;
    for (int i = text.size(); i < 3; i++) {
        text += "_";
    }
}

/**
* This method is called whenever the screen needs to be
* rendered. All pre and post steps are done by the app, so
* this function only needs to render its components
*/
void CreateScreen::render() {

    // Render the background
    TexMan::drawImage("assets/bg.png", 0, 0, 900, 600);

    // Render the header rect
    TexMan::drawRect({ 0, 0, 0, 180 }, 0, 0, 900, 90);

    // Render the back button
    TexMan::drawHoverImage("assets/back_btn.png", 20, 20, 100, 50, mx, my);

    // Render the center square
    TexMan::drawRect({ 0, 0, 0, 180 }, 300, 150, 300, 300);

    // Render the textbox
    TexMan::drawImage("assets/register_textbox.png", 350, 215, 200, 50);

    // Render the text
    TexMan::drawText(text, { 255, 255, 255, 255 }, 18, 450, 240);

    // Render the size buttons
    for (int i = 0; i < 5; i++) {

        int x = 350 + i * 42;
        int y = 283;

        if (sizes[i] == size) {
            TexMan::drawRect({ 255, 255, 255, 255 }, x-2, y-2, 36, 36);
        }

        // Render the textbox
        TexMan::drawHoverImage("assets/small_btn.png", x, y, 32, 32, mx, my);

        TexMan::drawText(to_string(sizes[i]), { 255, 255, 255, 255 }, 10, x+16, y+16);
    }

    // Render the button
    TexMan::drawHoverImage("assets/register_btn.png", 350, 335, 200, 50, mx, my);
}

/**
* This method is called whenever the screen needs to be cleaned
*/
void CreateScreen::clean() {

}