#include "screen_register.h"
#include "texman.h"
#include <iostream>
#include <string>
using namespace std;

/*
* This method is called when the game is first created. It
* is used to load all assets and objects needed for the
* screen.
*/
void RegisterScreen::init() {

    // Setup background
    texBG = TexMan::LoadImageTexture("assets/register_bg.png", renderer);
    rectBG.x = 0;
    rectBG.y = 0;
    rectBG.w = 900;
    rectBG.h = 600;

    // Setup button
    texBtn = TexMan::LoadImageTexture("assets/register_btn.png", renderer);
    rectBtn.x = rectBG.w / 2 - 100;
    rectBtn.y = rectBG.h / 2 - 25 + 30;
    rectBtn.w = 200;
    rectBtn.h = 50;

    // Setup text
    texName = TexMan::LoadImageTexture("assets/register_textbox.png", renderer);
    //font = TTF_OpenFont("assets/rubik.ttf", 24);
    username = "";
    rectName.x = rectBG.w / 2 - 100;
    rectName.y = rectBG.h / 2 - 25 - 30;
    rectName.w = 200;
    rectName.h = 50;
}

/**
* This method is called whenever an event happens and this
* screen is currently being rendered.
*/
void RegisterScreen::handleEvent(SDL_Event& event) {
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

        // Not in the box (horizontal)
        if (x < rectBtn.x || x > rectBtn.x + rectBtn.w) {
            return;
        }

        // Not in the box (vertical)
        if (y < rectBtn.y || y > rectBtn.y + rectBtn.h) {
            return;
        }

        // Open screen #1
        app->openScreen(1);
    }
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

    // Render the background
    SDL_RenderCopy(renderer, texBG, NULL, &rectBG);

    // Render the textbox
    SDL_RenderCopy(renderer, texName, NULL, &rectName);

    //this opens a font style and sets a size
    if (username.size() > 0) {
        // Get text texture
        SDL_Color textColor = { 255, 255, 255 };
        SDL_Texture* text = TexMan::LoadFontTexture(username.c_str(), textColor, 24, renderer);

        // Render it
        int text_width, text_height;
        SDL_QueryTexture(text, NULL, NULL, &text_width, &text_height);
        SDL_Rect textRect;
        textRect.x = rectName.x + (rectName.w - text_width) / 2;
        textRect.y = rectName.y + (rectName.h - text_height) / 2;
        textRect.w = text_width;
        textRect.h = text_height;
        SDL_RenderCopy(renderer, text, NULL, &textRect);
        SDL_DestroyTexture(text);
    }

    // Render the button
    SDL_RenderCopy(renderer, texBtn, NULL, &rectBtn);
}