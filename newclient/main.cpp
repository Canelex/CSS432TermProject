#include <iostream>
#include "app.h"
#include "SDL.h"
using namespace std;

App* app;

int main(int argc, char* argv[]) {

    // Frame delay
    const int FPS = 60;
    const int frameDelay = 1000 / FPS;
    Uint32 frameStart;
    int frameTime;

    // Create an app object, initialize it 167.172.239.146
    app = new App("50.35.76.95", 12345);
    app->init("Term Project", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 900, 600, false);

    // Start the application loop
    while (app->isRunning()) {
        // Record start time
        frameStart = SDL_GetTicks();

        // Input, update, render
        app->handleEvents();
        app->update();
        app->render();

        // Delay the game if we are running faster than FPS
        frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }

    // Cleanup the process
    app->clean();
	return 0;
}