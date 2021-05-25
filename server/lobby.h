//This class defines the lobby object which holds players and runs the game
#ifndef LOBBY
#define LOBBY
#include <string>
#include <iostream>
#include <vector>
#include "player.h"
using namespace std;

class lobby
{
public:
    vector<player> getPlayers();
    int getLobbyId();
    int getLobbySize();
    int getLobbyNumPlayers();

    void setLobbySize(int size);
    void setLobbyName(string name);
    void setLobbyId(int Id);
    void addPlayers(player newPlayer);
    void update();


private:
    string lobbyName;
    int lobbyId;
    vector<player> players;
    int lobbySize = 0;
    bool startGame;


};

#endif