#include "texman.h"

SDL_Texture* TexMan::LoadImageTexture(const char* file, SDL_Renderer* renderer) {
    // Load resources, create image
    SDL_Surface* tmp = IMG_Load(file);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, tmp);

    // Free resources
    SDL_FreeSurface(tmp);

    return tex;
}

SDL_Texture* TexMan::LoadFontTexture(const char* text, SDL_Color& color, int fontSize, SDL_Renderer* renderer) {

    // Load resources, create text
    TTF_Font* font = TTF_OpenFont("assets/rubik.ttf", fontSize);
    SDL_Surface* tmp = TTF_RenderText_Solid(font, text, color);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, tmp);

    // Release resources
    SDL_FreeSurface(tmp);
    TTF_CloseFont(font);

    return tex;
}