#include "screen_register.h"
#include <iostream>
#include <string>
using namespace std;

/*
* This method is called when the game is first created. It
* is used to load all assets and objects needed for the
* screen.
*/
void RegisterScreen::init() {
    // Reset name
    username = "";
    text = "";
}

/**
* This method is called whenever an event happens and this
* screen is currently being rendered.
*/
void RegisterScreen::handleEvent(SDL_Event& event) {
    if (event.type == SDL_MOUSEMOTION) {
        mx = event.button.x;
        my = event.button.y;
    }

    // Key press
    if (event.type == SDL_KEYDOWN) {
        string key = SDL_GetKeyName(event.key.keysym.sym);

        // Erase key
        if (key == "Backspace" && username.size() > 0) {
            username = username.substr(0, username.size() - 1);
        }

        // Key is too long
        if (key.size() > 1) {
            return;
        }

        // Name is too long
        if (username.size() >= 10) {
            return;
        }

        // Valid key
        if ((key >= "A" && key <= "Z") || (key >= "0" && key <= "9")) {
            username += key;
        }
    }

    // Mouse click
    if (event.type == SDL_MOUSEBUTTONDOWN) {

        // We only care about left clicks
        if (event.button.button != SDL_BUTTON_LEFT) {
            return;
        }

        // Make sure name is valid
        if (username.size() < 3) {
            return;
        }

        // Was it in the box?
        int x = event.button.x;
        int y = event.button.y;

        // box 350, 305, 200, 50

        // Not in the box (horizontal)
        if (x < 350 || x > 350 + 200) {
            return;
        }

        // Not in the box (vertical)
        if (y < 305 || y > 305 + 50) {
            return;
        }

        // Dispatch a register packet
        app->getNetworkManager()->sendRegister(username);
    }
}

/**
* This method is called whenever a packet is received from
* the app.
*/
void RegisterScreen::handlePacket(string packet) {
    switch (packet.at(0)) {
        case 'R':
            if (packet == "RT\n") {
                cout << "Successfully registered" << endl;
                app->openScreen(1);
            }
            if (packet == "RF\n") {
                cerr << "Failed to register" << endl;
                username = "";
            }
            break;
    }
}

/**
* If there is any kind of logic that needs to run per tick,
* this is the place to do it. This function fires 30 times
* a second (once per frame) when the screen is active
*/
void RegisterScreen::update() {
    // Update text
    text = username;
    for (int i = text.size(); i < 3; i++) {
        text += "_";
    }
}

/**
* This method is called whenever the screen needs to be
* rendered. All pre and post steps are done by the app, so
* this function only needs to render its components
*/
void RegisterScreen::render() {

    // Render the background
    TexMan::drawImage("assets/register_bg.png", 0, 0, 900, 600);

    // Render the center square
    TexMan::drawRect({ 17, 76, 122, 255 }, 300, 150, 300, 300);

    // Render the textbox
    TexMan::drawImage("assets/register_textbox.png", 350, 245, 200, 50);

    // Render the text
    TexMan::drawText(text, { 255, 255, 255, 255 }, 24, 450, 270);

    // Render the button
    TexMan::drawHoverImage("assets/register_btn.png", 350, 305, 200, 50, mx, my);
}

/**
* This method is called whenever the screen needs to be cleaned
*/
void RegisterScreen::clean() {}