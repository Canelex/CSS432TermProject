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
#include "netman.h"

/**
* Constructor accepts a server address and a port, which it eventually
* passes to the network manager (which uses a thread to do networking)
*/
App::App(const char* address, int port) {
    netman = new NetMan(address, port);
}

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
            cout << "Renderer created!" << endl;
        }

        if (TTF_Init() == 0) {
            cout << "Font initialized!" << endl;
        }

        // Setup texman
        TexMan::setRenderer(renderer);

        closing = false;
        running = true;
    }
    else {
        running = false;
    }

    // Create some screens
    activeScreenIndex = -1;
    screens.push_back(new RegisterScreen(this)); // 0
    screens.push_back(new LobbiesScreen(this)); // 1
    screens.push_back(new CreateScreen(this)); // 2
    screens.push_back(new LobbyScreen(this)); // 3
    screens.push_back(new GameScreen(this)); // 4
    openScreen(0);
}

/**
* This function handles all user input to the system. It is called every
* tick by the main application loop.
*/
void App::handleEvents() {
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            if (!closing && netman->isConnected()) {
                // Try to disconnect
                netman->sendQuitGame();
                closing = true;
            } else {
                // Okay, just close it.
                running = false;
            }
            break;
        default:
            screens[activeScreenIndex]->handleEvent(event);
            break;
        }
    }
}

/**
* This funciton handles all packets from the network manager to the client.
* It is called by the update function.
*/
void App::handlePackets() {
    while (netman->hasNextPacket()) {
        string p = netman->poll();
        if (p.length() > 0) {

            // Game closed?
            if (p == "close") {
                running = false;
                return;
            }

            screens[activeScreenIndex]->handlePacket(p);
        }
    }
}

/**
* This function handles all the game logic. It is called every tick by
* the main application loop.
*/
void App::update() {
    // Dequeue some packets
    handlePackets();

    // Update screen
    screens[activeScreenIndex]->update();

    // Change title to say if connected
    if (netman->isConnected()) {
        SDL_SetWindowTitle(window, "Term Project [Connected]");
    } else {
        SDL_SetWindowTitle(window, "Term Project [Disconnected]");
    }
}

/**
* This function is responsible for rendering to the screen. It is called
* every tick by the main loop, after inputs & updates have been ran.
*/
void App::render() {
    // Select white
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // Clear screen w/ white
    SDL_RenderClear(renderer);

    // Have the active screen render
    screens[activeScreenIndex]->render();

    // Update renderer
    SDL_RenderPresent(renderer);
}

/**
* This function is responsible for cleaning up the application. It is called
* by the main process after the game has completed.
*/
void App::clean() {
    // Destroy the window
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();

    // Deallocate the netman
    if (netman != nullptr) {
        delete netman;
        netman = nullptr;
    }

    cout << "Game cleaned." << endl;
}

/**
* This function opens another screen. It accepts an id parameter which switches
* the activeScreenIndex field
*/
void App::openScreen(int id) {
    if (activeScreenIndex > 0) {
        screens[activeScreenIndex]->clean();
    }

    // Update the index
    activeScreenIndex = id;

    // Initialize the screen
    screens[activeScreenIndex]->init();
}

/**
* Returns whether the application is still running.
*/
bool App::isRunning() const {
    return running;
}

/**
* Sets the lobby id
*/
void App::setLobby(int id, string name, int maxsize) {
    lobbyId = id;
    lobbyName = name;
    lobbyMaxSize = maxsize;
}

/**
* Gets the lobby id
*/
int App::getLobbyId() {
    return lobbyId;
}

/**
* Returns the lobby name
*/
string App::getLobbyName() {
    return lobbyName;
}

/**
* Gets the lobby maxsize
*/
int App::getLobbyMaxSize() {
    return lobbyMaxSize;
}

/**
* Sets the player id
*/
void App::setPlayerId(int id) {
    playerId = id;
}

/**
* Gets the player id
*/
int App::getPlayerId() {
    return playerId;
}

/**
* Returns the network manager
*/
NetMan* App::getNetworkManager() {
    return netman;
}