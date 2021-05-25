#include "screen_register.h"
#include "texman.h"
#include "SDL_ttf.h"
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
    texBG = TexMan::LoadTexture("assets/register_bg.png", renderer);
    rectBG.x = 0;
    rectBG.y = 0;
    rectBG.w = 900;
    rectBG.h = 600;

    // Setup button
    texBtn = TexMan::LoadTexture("assets/register_btn.png", renderer);
    rectBtn.x = rectBG.w / 2 - 100;
    rectBtn.y = rectBG.h / 2 - 25 + 30;
    rectBtn.w = 200;
    rectBtn.h = 50;

    // Setup text
    texName = TexMan::LoadTexture("assets/register_textbox.png", renderer);
    font = TTF_OpenFont("assets/rubik.ttf", 24);
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
    if (event.type == SDL_KEYDOWN) {
        string key = SDL_GetKeyName(event.key.keysym.sym);

        if (key.size() == 1 && key >= "A" && key <= "Z" && username.size() < 10) {
            username += key;
        }
        if (key == "Backspace" && username.size() > 0) {
            username = username.substr(0, username.size() - 1);
        }
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
        SDL_Color textColor = { 255, 255, 255 };
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, username.c_str(), textColor);
        SDL_Texture* text = SDL_CreateTextureFromSurface(renderer, textSurface);
        int text_width = textSurface->w;
        int text_height = textSurface->h;
        SDL_Rect textRect;
        textRect.x = rectName.x + (rectName.w - text_width) / 2;
        textRect.y = rectName.y + (rectName.h - text_height) / 2;
        textRect.w = text_width;
        textRect.h = text_height;
        SDL_FreeSurface(textSurface);
        SDL_RenderCopy(renderer, text, NULL, &textRect);
        SDL_DestroyTexture(text);
    }

    // Render the button
    SDL_RenderCopy(renderer, texBtn, NULL, &rectBtn);
}