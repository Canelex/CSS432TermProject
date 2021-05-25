#pragma once
#include "app.h"

class TexMan
{
public:
    static SDL_Texture* LoadTexture(const char* file, SDL_Renderer* renderer);
};

