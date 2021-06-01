#pragma once
#include "app.h"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include <string>
#include <map>
using namespace std;

class TexMan
{
public:
    /**
    * This function sets the renderer being used by texman.
    */
    static void setRenderer(SDL_Renderer* renderer);

    /**
    * This function renders an image using the current renderer.
    * Does caching using the textures map
    */
    static void drawImage(string path, int x, int y, int w, int h);

    /**
    * This function renders an image using the current renderer.
    * Does caching using the textures map
    */
    static void drawHoverImage(string path, int x, int y, int w, int h, int mx, int my);

    /**
    * This function renders a rectangle using the current renderer.
    */
    static void drawRect(SDL_Color color, int x, int y, int w, int h);

    /**
    * This function renders text using the current renderer.
    * Text will be rendered centered on x and y.
    */
    static void drawText(string text, SDL_Color color, int fontSize, int x, int y);

    /**
    * This function cleans the cache and font.
    */
    static void cleanup();

private:
    // Cached textures
    static map<string, SDL_Texture*> textures;

    // Reference to renderer
    static SDL_Renderer* renderer;
};

