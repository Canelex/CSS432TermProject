#include "screen_create.h"

/*
* This method is called when the game is first created. It
* is used to load all assets and objects needed for the
* screen.
*/
void CreateScreen::init() {}

/**
* This method is called whenever an event happens and this
* screen is currently being rendered.
*/
void CreateScreen::handleEvent(SDL_Event& event) {
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
    }
}

/**
* This method is called whenever a packet is received from
* the app.
*/
void CreateScreen::handlePacket(string packet) {

}

/**
* If there is any kind of logic that needs to run per tick,
* this is the place to do it. This function fires 30 times
* a second (once per frame) when the screen is active
*/
void CreateScreen::update() {

}

/**
* This method is called whenever the screen needs to be
* rendered. All pre and post steps are done by the app, so
* this function only needs to render its components
*/
void CreateScreen::render() {

    // Render the background
    TexMan::drawImage("assets/register_bg.png", 0, 0, 900, 600);

    // Render the header rect
    TexMan::drawRect({ 17, 76, 122, 255 }, 0, 0, 900, 90);

    // Render the back button
    TexMan::drawImage("assets/back_btn.png", 20, 20, 100, 50);
}

/**
* This method is called whenever the screen needs to be cleaned
*/
void CreateScreen::clean() {

}