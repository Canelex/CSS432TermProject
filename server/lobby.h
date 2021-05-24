#pragma once
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