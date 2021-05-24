#pragma once
#include <string>
#include <iostream>

using namespace std;

class player
{
public:
    string getPlayerName();
    int getPlayerSocket();
    int getPlayerXPos();
    int getPlayerYPos();
    void* getPlayerColor();

    void setName(string name);
    void setSocket(int socket);
    void setXPos(int x);
    void setYPos(int y);
    void setColor(void* color);


private:
    string name = "";
    int sockFd = 0;
    int x = 0;
    int y = 0;
    void* color;
};