//This class defines the player object which interacts with lobbies and the gameplay
#ifndef PLAYER
#define PLAYER
#include <string>
#include <iostream>

//using namespace std;

class player
{
public:
    std::string getPlayerName();
    int getPlayerSocket();
    int getPlayerXPos();
    int getPlayerYPos();
    void* getPlayerColor();

    void setName(std::string name);
    void setSocket(int socket);
    void setXPos(int x);
    void setYPos(int y);
    void setColor(void* color);


private:
    std::string name = "";
    int sockFd = -1;
    int x = 0;
    int y = 0;
    void* color;
};

#endif