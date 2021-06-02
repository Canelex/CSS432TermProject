//This class defines the lobby object which holds players and runs the game
#ifndef LOBBY
#define LOBBY
#include <string>
#include <iostream>
#include <vector>
#include <sys/time.h>
#include "player.h"
//using namespace std;

class player;

class lobby
{
public:
    std::vector<player*> getPlayers();
    std::string getLobbyName();
    int getLobbyId();
    int getLobbySize();
    int getLobbyNumPlayers();
    std::string getAlivePlayersPos();

    void setLobbySize(int size);
    void setLobbyName(std::string name);
    void setLobbyId(int Id);

    void addPlayer(player* newPlayer);
    void updatePlayersPos();
    void pushPlayersInGame();
    player* popPlayer(player* p);
    bool findPlayer(player* p);
    bool checkForWinner();
    player* getWinner();

    timeval lobbyTV;


private:
    std::string lobbyName;
    int lobbyId;
    std::vector<player*> players;
    int lobbySize = 0;
    bool startGame;


};

#endif