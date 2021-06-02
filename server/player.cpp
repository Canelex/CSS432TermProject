#include "player.h"

std::string player::getPlayerName()
{
    return this->name;
}

int player::getPlayerSocket()
{
    return this->sockFd;
}

int player::getPlayerId()
{
    return this->playerId;
}

lobby* player::getMyLobby()
{
    return this->myLobby;
}

int player::getPlayerXPos()
{
    return this->x;
}

int player::getPlayerYPos()
{
    return this->y;
}

int player::getDirection()
{
    return this->direction;
}

bool player::isInLobby()
{
    return this->inLobby;
}

bool player::isInGame()
{
    return this->inGame;
}

bool player::isAlive()
{
    return this->alive;
}

void player::setName(std::string name)
{
    this->name = name;
}

void player::setSocket(int socket)
{
    this->sockFd = socket;
}

void player::setPlayerId(int id)
{
    this->playerId = id;
}

void player::setMyLobby(lobby* l)
{
    this->myLobby = l;
}

void player::setXPos(int x)
{
    this->x = x;
}

void player::setYPos(int y)
{
    this->y = y;
}

void player::setDirection(int dir)
{
    this->direction = dir;
}

void player::setInLobby(bool b)
{
    this->inLobby = b;
}

void player::setInGame(bool b)
{
    this->inGame = b;
}

void player::setAlive(bool b)
{
    this->alive = b;
}