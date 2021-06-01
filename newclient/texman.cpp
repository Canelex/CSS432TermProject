#include "texman.h"

/**
* This function sets the renderer being used by texman.
*/
void TexMan::setRenderer(SDL_Renderer* renderer) {
    TexMan::renderer = renderer;
}

/**
* This function renders an image using the current renderer.
* Does caching using the textures map
*/
void TexMan::drawImage(string path, int x, int y, int w, int h) {
    // Get or load the texture
    SDL_Texture* texture;
    map<string,SDL_Texture*>::const_iterator pos = textures.find(path);

    if (pos == textures.end()) {
        // Doesn't have it.
        SDL_Surface* tmp = IMG_Load(path.c_str());
        texture = SDL_CreateTextureFromSurface(renderer, tmp);
        SDL_FreeSurface(tmp);

        // Save it now
        cout << "Loaded texture " << path << endl;
        textures.insert(make_pair(path, texture));
    } else {
        // Get texture from map
        texture = pos->second;
    }

    // Render it
    SDL_Rect rect = { x, y, w, h };
    SDL_RenderCopy(renderer, texture, NULL, &rect);
}

/**
* This function renders a rectangle using the current renderer.
*/
void TexMan::drawRect(SDL_Color color, int x, int y, int w, int h) {
    // Render the rect
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_Rect rect = { x, y, w, h };
    SDL_RenderFillRect(renderer, &rect);
}

/**
* This function renders text using the current renderer.
* Text will be rendered centered on x and y.
*/
void TexMan::drawText(string text, SDL_Color color, int fontSize, int x, int y) {
    // Load font if it isnt already
    TTF_Font* font = TTF_OpenFont("assets/rubik.ttf", fontSize);

    // Create the text
    SDL_Surface* tmp = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, tmp);
    int tw = tmp->w;
    int th = tmp->h;

    // Render the text
    SDL_Rect rect = { x - tw / 2, y - th / 2, tw, th };
    SDL_RenderCopy(renderer, tex, NULL, &rect);

    // Delete surface and texture
    SDL_FreeSurface(tmp);
    SDL_DestroyTexture(tex);
    TTF_CloseFont(font);
}

/**
* This function cleans the cache.
*/
void TexMan::cleanup() {

}

// Cached textures
map<string, SDL_Texture*> TexMan::textures;

// Reference to renderer
SDL_Renderer* TexMan::renderer = nullptr;