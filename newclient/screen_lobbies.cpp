#include "screen_lobbies.h"
#include "netman.h"

/*
* This method is called when the game is first created. It
* is used to load all assets and objects needed for the
* screen.
*/
void LobbiesScreen::init() {
    // Setup background
    texBG = TexMan::LoadImageTexture("assets/register_bg.png", renderer);
    rectBG.x = 0;
    rectBG.y = 0;
    rectBG.w = 900;
    rectBG.h = 600;

    // Send a list lobbies
    app->getNetworkManager().dispatch("L");
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

    }
}

/**
* This method is called whenever a packet is received from
* the app.
*/
void LobbiesScreen::handlePacket(string packet) {
    switch (packet.at(0)) {
    case 'L':
        cout << packet << endl;
        break;
    }
}

/**
* If there is any kind of logic that needs to run per tick,
* this is the place to do it. This function fires 30 times
* a second (once per frame) when the screen is active
*/
void LobbiesScreen::update() {

}

/**
* This method is called whenever the screen needs to be
* rendered. All pre and post steps are done by the app, so
* this function only needs to render its components
*/
void LobbiesScreen::render() {

    // Render the background
    SDL_RenderCopy(renderer, texBG, NULL, &rectBG);



}

/**
* This method is called whenever the screen needs to be cleaned
*/
void LobbiesScreen::clean() {

}