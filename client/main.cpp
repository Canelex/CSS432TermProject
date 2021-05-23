#include <iostream>
#include "game.h"

using namespace std;

Game* game = nullptr;

int main(int argc, char* argv[]) {
	cout << "Game started" << endl;

	game = new Game();
	game->init("Term Project", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, false);

	
	while (game->running()) {
		game->handleEvents();
		game->update();
		game->render();
	}

	game->clean();
	return 0;
}