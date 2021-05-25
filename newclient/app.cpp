/**
* app.h/cpp
* 5/23/2021
*
* This class is responsible for all things related to the application and
* game. This includes including input, rendering, and updating for screens
* such as register, game selection, and playing. It uses the class ____ for
* networking and connecting to the server.
*/

#include "app.h"
#include "texman.h"

SDL_Texture* texture;
SDL_Rect srcR, destR;

/* Constructor and destructor */
App::App() {}

App::~App() {}

/* Initialization function for starting the game */
void App::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
        cout << "Subsystems initialized." << endl;

        // Create flags
        int flags = 0;
        if (fullscreen) {
            flags = SDL_WINDOW_FULLSCREEN;
        }

        // Create a window
        window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
        if (window) {
            cout << "Window created!" << endl;
        }

        // Create a renderer
        renderer = SDL_CreateRenderer(window, -1, 0);
        if (renderer) {
            // Set renderer color to white
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            cout << "Renderer created!" << endl;
        }

        // Load some textures
        texture = TexMan::LoadTexture("assets/test.png", renderer);

        running = true;
    }
    else {
        running = false;
    }
}

/**
* This function handles all user input to the system. It is called every
* tick by the main application loop.
*/
void App::handleEvents() {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {
    case SDL_QUIT:
        running = false;
        break;
    default:
        break;
    }
}

/**
* This function handles all the game logic. It is called every tick by
* the main application loop.
*/
void App::update() {
    destR.w = 50;
    destR.h = 50;
    destR.x++;
}

/**
* This function is responsible for rendering to the screen. It is called
* every tick by the main loop, after inputs & updates have been ran.
*/
void App::render() {
    // Clear renderer
    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, texture, NULL, &destR);

    // Update renderer
    SDL_RenderPresent(renderer);
}

/**
* This function is responsible for cleaning up the application. It is called
* by the main process after the game has completed.
*/
void App::clean() {
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    cout << "Game cleaned." << endl;
}

/**
* Returns whether the application is still running.
*/
bool App::isRunning() const {
    return running;
}