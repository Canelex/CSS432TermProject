#include "texman.h"

SDL_Texture* TexMan::LoadTexture(const char* file, SDL_Renderer* renderer) {
    SDL_Surface* tmp = IMG_Load(file);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, tmp);
    SDL_FreeSurface(tmp);
    return tex;
}