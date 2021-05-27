#pragma once
#include "app.h"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

class TexMan
{
public:
    static SDL_Texture* LoadImageTexture(const char* file, SDL_Renderer* renderer);

    static SDL_Texture* LoadFontTexture(const char* text, SDL_Color& color, int fontSize, SDL_Renderer* renderer);
};

