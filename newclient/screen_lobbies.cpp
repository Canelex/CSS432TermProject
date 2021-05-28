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

    // Setup back button
    texBack = TexMan::LoadImageTexture("assets/back_btn.png", renderer);
    rectBack.x = 20;
    rectBack.y = 20;
    rectBack.w = 100;
    rectBack.h = 50;

    // Setup lobby button
    texLobby = TexMan::LoadImageTexture("assets/lobby_btn.png", renderer);

    // Setup custom button
    texCustom = TexMan::LoadImageTexture("assets/custom_btn.png", renderer);
    rectCustom = { 430, 20, 100, 50 };

    // Send a list lobbies
    //app->getNetworkManager().dispatch("L");
    lobbies.push_back({ 0, "Lobby 1" });
    lobbies.push_back({ 1, "Lobby 2" });
    lobbies.push_back({ 2, "Lobby 3" });
    lobbies.push_back({ 2, "Lobby 4" });
    lobbies.push_back({ 2, "Lobby 5" });

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

        if (x >= rectBack.x && x <= rectBack.x + rectBack.w &&
            y >= rectBack.y && y <= rectBack.y + rectBack.h) {

            app->openScreen(0);
        }

        if (x >= rectCustom.x && x <= rectCustom.x + rectCustom.w &&
            y >= rectCustom.y && y <= rectCustom.y + rectCustom.h) {

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
    
}

/**
* This method is called whenever the screen needs to be
* rendered. All pre and post steps are done by the app, so
* this function only needs to render its components
*/
void LobbiesScreen::render() {

    // Render the background
    SDL_RenderCopy(renderer, texBG, NULL, &rectBG);

    SDL_SetRenderDrawColor(renderer, 17, 76, 122, 255);
    SDL_Rect rectScroll = { 0, 0, 550, 600 };
    SDL_RenderFillRect(renderer, &rectScroll);

    SDL_Rect rect = { 50, 120, 200, 200 };
    
    for (int i = 0; i < lobbies.size(); i++) {
        rect.x = 50 + 250 * (i % 2);
        rect.y = 120 + 250 * (int)(i / 2) - scrollY;
        SDL_RenderCopy(renderer, texLobby, NULL, &rect);

        // Get text texture
        SDL_Color textColor = { 255, 255, 255 };
        SDL_Texture* text = TexMan::LoadFontTexture(lobbies[i].name.c_str(), textColor, 24, renderer);

        // Render it
        int text_width, text_height;
        SDL_QueryTexture(text, NULL, NULL, &text_width, &text_height);
        SDL_Rect textRect;
        textRect.x = rect.x + (rect.w - text_width) / 2;
        textRect.y = rect.y + (rect.h - text_height) / 2;
        textRect.w = text_width;
        textRect.h = text_height;
        SDL_RenderCopy(renderer, text, NULL, &textRect);
        SDL_DestroyTexture(text);
    }

    SDL_Rect rectHeader = { 0, 0, 550, 90 };
    SDL_RenderFillRect(renderer, &rectHeader);

    // Render the back button
    SDL_RenderCopy(renderer, texBack, NULL, &rectBack);

    // Render the custom button
    SDL_RenderCopy(renderer, texCustom, NULL, &rectCustom);

}

/**
* This method is called whenever the screen needs to be cleaned
*/
void LobbiesScreen::clean() {
    lobbies.clear();
}