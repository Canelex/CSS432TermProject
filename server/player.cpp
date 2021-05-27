#include "player.h"

std::string player::getPlayerName()
{
    return this->name;
}

int player::getPlayerSocket()
{
    return this->sockFd;
}

int player::getPlayerXPos()
{
    return this->x;
}

int player::getPlayerYPos()
{
    return this->y;
}

void* player::getPlayerColor()
{
    return this->color;
}

void player::setName(std::string name)
{
    this->name = name;
}

void player::setSocket(int socket)
{
    this->sockFd = socket;
}

void player::setXPos(int x)
{
    this->x = x;
}

void player::setYPos(int y)
{
    this->y = y;
}

void player::setColor(void* color)
{
    this->color = color;
}