#include "lobby.h"

vector<player> lobby::getPlayers()
{
    return this->players;
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
    
void lobby::setLobbySize(int size)
{
    this->lobbySize = size;
    return;
}

void lobby::setLobbyName(string name)
{
    this->lobbyName = name;
}

void lobby::setLobbyId(int Id)
{
    this->lobbyId = Id;
}

void lobby::addPlayers(player newPlayer)
{
    this->players.push_back(newPlayer);
}

void update();