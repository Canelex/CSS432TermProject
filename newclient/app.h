/**
* app.h/cpp
* 5/23/2021
*
* This class is responsible for all things related to the application and
* game. This includes including input, rendering, and updating for screens
* such as register, game selection, and playing. It uses the class ____ for
* networking and connecting to the server.
*/

#pragma once
#include <iostream>
#include <vector>
#include <SDL.h>
#include "screen.h"
#include "screen_register.h"
#include "screen_lobbies.h"
#include "screen_create.h"
#include "screen_lobby.h"
#include "screen_game.h"
#include "netman.h"
using namespace std;

// Define screen ahead of time
class Screen;

class App
{
public:

    /**
    * Constructor accepts a server address and a port, which it eventually
    * passes to the network manager (which uses a thread to do networking)
    */
    App(const char* address, int port);

    /* Initialization function for starting the game */
    void init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);

    /**
    * This function handles all user input to the system. It is called every
    * tick by the main application loop.
    */
    void handleEvents();

    /**
    * This funciton handles all packets from the network manager to the client.
    * It is called by the update function.
    */
    void handlePackets();

    /**
    * This function handles all the game logic. It is called every tick by
    * the main application loop.
    */
    void update();

    /**
    * This function is responsible for rendering to the screen. It is called
    * every tick by the main loop, after inputs & updates have been ran.
    */
    void render();

    /**
    * This function is responsible for cleaning up the application. It is called
    * by the main process after the game has completed.
    */
    void clean();

    /**
    * This function opens another screen. It accepts an id parameter which switches
    * the activeScreenIndex field
    */
    void openScreen(int id);

    /**
    * Returns whether the application is still running.
    */
    bool isRunning() const;

    /**
    * Sets the lobby id
    */
    void setLobby(int id, string name, int maxsize);

    /**
    * Gets the lobby id
    */
    int getLobbyId();

    /**
    * Returns the lobby name
    */
    string getLobbyName();

    /**
    * Gets the lobby maxsize
    */
    int getLobbyMaxSize();

    /**
    * Sets the player id
    */
    void setPlayerId(int id);

    /**
    * Gets the player id
    */
    int getPlayerId();


    /**
    * Returns the network manager
    */
    NetMan* getNetworkManager();

private:
    // Is the game running?
    bool running;
    bool closing;

    // vector of screens
    vector<Screen*> screens;

    // Which screen is active?
    int activeScreenIndex;

    // Which lobby ID do I have?
    int lobbyId;
    string lobbyName;
    int lobbyMaxSize;

    int playerId;

    // NetMan instance
    NetMan* netman;

    // SDL window pointer
    SDL_Window* window;

    // SDL renderer pointer
    SDL_Renderer* renderer;
};

