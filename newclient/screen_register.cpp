#include "screen_register.h"
#include <iostream>
using namespace std;

/*
* This method is called when the game is first created. It
* is used to load all assets and objects needed for the
* screen.
*/
void RegisterScreen::init() {

    // Setup button
    rectBtn.x = 0;
    rectBtn.y = 0;
    rectBtn.w = 50;
    rectBtn.h = 50;
}

/**
* This method is called whenever an event happens and this
* screen is currently being rendered.
*/
void RegisterScreen::handleEvent(SDL_Event& event) {

}

/**
* If there is any kind of logic that needs to run per tick,
* this is the place to do it. This function fires 30 times
* a second (once per frame) when the screen is active
*/
void RegisterScreen::update() {

}

/**
* This method is called whenever the screen needs to be
* rendered. All pre and post steps are done by the app, so
* this function only needs to render its components
*/
void RegisterScreen::render() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    
    SDL_RenderFillRect(renderer, &rectBtn);
}