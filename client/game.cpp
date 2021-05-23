/**
 * game.h/cpp
 * 5/23/2021
 *
 * This class is responsible for all things related to the game itself,
 * including input, rendering, and updating.It uses the class ____ for
 * networking and connecting to the server.
 */
#include "game.h"

Game::Game() {}

Game::~Game() {}

void Game::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen) {
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
			cout << "Renderer created!" << endl;
		}

		isRunning = true;
	} else {
		isRunning = false;
	}
}

void Game::handleEvents() {
	SDL_Event event;
	SDL_PollEvent(&event);

	switch (event.type) {
		case SDL_QUIT:
			isRunning = false;
			break;
		default:
			break;
	}
}

void Game::update() {

}

void Game::render() {
	SDL_RenderClear(renderer);
}

void Game::clean() {
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	cout << "Game cleaned." << endl;
}

bool Game::running() const {
	return this->isRunning;
}