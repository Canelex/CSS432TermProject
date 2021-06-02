#include "lobby.h"
#include <iostream>

std::vector<player*> lobby::getPlayers()
{
    return this->players;
}

std::string lobby::getLobbyName()
{
    return this->lobbyName;
}

int lobby::getLobbyId()
{
    return this->lobbyId;
}

int lobby::getLobbySize()
{
    return this->lobbySize;
}

int lobby::getLobbyNumPlayers()
{
    return this->players.size();
}

std::string lobby::getAlivePlayersPos()
{
    std::string message;
    for(player* p: this->players)
    {
        if(p->isAlive() && p->isInGame())
        {
            message += "M/";
            message += std::to_string(p->getPlayerId());
            message += "/";
            message += std::to_string(p->getPlayerXPos());
            message += "/";
            message += std::to_string(p->getPlayerYPos());
            message += "\n";
        }
    }
    return message;
}

player* lobby::popPlayer(player* p)
{
    int i = 0;
    for(player* indP: this->players)
    {
        if(indP->getPlayerSocket() == p->getPlayerSocket())
        {
            this->players.erase(this->players.begin() + i);
            p->setMyLobby(NULL);
            p->setInLobby(false);
            return p;
        }
        i++;
    }
    return NULL;
}

bool lobby::findPlayer(player* p)
{
    for(player* indP: this->players)
    {
        if(indP->getPlayerSocket() == p->getPlayerSocket())
        {
            return true;
        }
    }
    return false;
}
    
void lobby::setLobbySize(int size)
{
    this->lobbySize = size;
    return;
}

void lobby::setLobbyName(std::string name)
{
    this->lobbyName = name;
}

void lobby::setLobbyId(int Id)
{
    this->lobbyId = Id;
}

void lobby::addPlayer(player* newPlayer)
{
    this->players.push_back(newPlayer);
    newPlayer->setMyLobby(this);
    newPlayer->setInLobby(true);
}

void lobby::updatePlayersPos()
{
    for(player* p: this->players)
    {
        int newPos = -1;
        switch(p->getDirection())
        {
            case 0:
                newPos = p->getPlayerYPos();
                newPos -= 1;
                p->setYPos(newPos);
                break;
            case 1:
                newPos = p->getPlayerXPos();
                newPos += 1;
                p->setXPos(newPos);
                break;
            case 2:
                newPos = p->getPlayerYPos();
                newPos += 1;
                p->setYPos(newPos);
                break;
            default:
                newPos = p->getPlayerXPos();
                newPos -= 1;
                p->setXPos(newPos);
                break;
        }

        if (p->getPlayerXPos() >= 50) {
            p->setXPos(0);
        }
        if (p->getPlayerYPos() >= 50) {
            p->setYPos(0);
        }
        if (p->getPlayerXPos() < 0) {
            p->setXPos(49);
        }
        if (p->getPlayerYPos() < 0) {
            p->setYPos(49);
        }

        std::cout << p->getPlayerName() << " is at " << p->getPlayerXPos() << "," << p->getPlayerYPos() << std::endl;
    }
}

void lobby::pushPlayersInGame()
{
    for(player* p: this->players)
    {
        p->setInGame(true);
        p->setAlive(true);
    }
}

bool lobby::checkForWinner()
{
    int deadPlayers = 0;
    for(player* p: this->players)
    {
        if(!p->isAlive())
        {
            deadPlayers++;
        }
        if(this->players.size() - deadPlayers <= 1)
        {
            return true;
        }
    }
    return false;
}

player* lobby::getWinner()
{
    for(player* p: this->players)
    {
        if(p->isAlive() && p->isInGame())
        {
            return p;
        }
    }
    return NULL;
}