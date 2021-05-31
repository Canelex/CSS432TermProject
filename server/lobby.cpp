#include "lobby.h"

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

player* lobby::findPlayerPop(player* p)
{
    int i = 0;
    for(player* indP: this->players)
    {
        if(indP->getPlayerSocket() == p->getPlayerSocket())
        {
            this->players.erase(this->players.begin() + i);
            return p;
        }
        i++;
    }
    return NULL;
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
}

void update();