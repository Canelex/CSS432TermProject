//This class defines the player object which interacts with lobbies and the gameplay
#ifndef PLAYER
#define PLAYER
#include <string>
#include <iostream>
#include "lobby.h"

//using namespace std;

class lobby;

class player
{
public:
    std::string getPlayerName();
    int getPlayerSocket();
    int getPlayerId();
    lobby* getMyLobby();
    int getPlayerXPos();
    int getPlayerYPos();
    int getDirection();
    bool isInLobby();
    bool isInGame();
    bool isAlive();
    
    void setName(std::string name);
    void setSocket(int socket);
    void setPlayerId(int id);
    void setMyLobby(lobby* l);
    void setXPos(int x);
    void setYPos(int y);
    void setDirection(int dir);
    void setInLobby(bool b);
    void setInGame(bool b);
    void setAlive(bool b);


private:
    std::string name = "";
    int sockFd = -1;
    int playerId = -1;
    lobby* myLobby = NULL;
    int x = 0;
    int y = 0;
    int direction = -1;
    bool inLobby = false;
    bool inGame = false;
    bool alive = true;
};

#endif