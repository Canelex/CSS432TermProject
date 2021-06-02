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

void lobby::update()
{

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
        if(this->players.size() - deadPlayers == 1)
        {
            return true;
        }
    }
    return false;
}