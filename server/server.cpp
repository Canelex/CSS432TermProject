#include <iostream>
#include <sys/types.h>    // socket, bind
#include <sys/socket.h>   // socket, bind, listen, inet_ntoa
#include <netinet/in.h>   // htonl, htons, inet_ntoa
#include <arpa/inet.h>    // inet_ntoa
#include <netdb.h>     // gethostbyname
#include <unistd.h>    // read, write, close
#include <strings.h>     // bzero
#include <netinet/tcp.h>  // SO_REUSEADDR
#include <sys/uio.h>      // writev
#include <cstring>
#include <thread>
#include <pthread.h>
#include <string.h>
#include <sys/time.h>
#include <random>
#include "lobby.h"
//using namespace std;

const unsigned int BUF_SIZE = 65535;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int PLAYER_ID = 0;

std::vector<lobby*> lobbies;

//void* threadConnection(void* newSd);

void* serviceConnection(void* newSd);

void addLobby(lobby* l);

player* registration(char* buf, int& fd);

void listLobbies(player* p);

void createLobby(char* buf, player* p);

void lobbyInfo(char* buf, player* p);

void joinLobby(char* buf, player* p);

void exitLobby(player* p);

void startGame(char* buf, player* p);

void sendGameState(char* buf, player* p);

void changeDirection(char* buf, player* p);

void death(char* buf, player* p);

void updateNumLobbyPlayers(lobby* l);

int lobbyIDGen();

int main () 
{
    lobby* lobby0 = new lobby();
    lobby* lobby1 = new lobby();
    lobby* lobby2 = new lobby();
    lobby0->setLobbySize(2);
    lobby0->setLobbyName("Preset 1v1");
    lobby0->setLobbyId(0);
    lobby1->setLobbySize(3);
    lobby1->setLobbyName("Preset 1v1v1");
    lobby1->setLobbyId(1);
    lobby2->setLobbySize(4);
    lobby2->setLobbyName("Preset 1v1v1v1");
    lobby2->setLobbyId(2);
    addLobby(lobby0);
    addLobby(lobby1);
    addLobby(lobby2);

    // Create the socket
    int server_port = 12345;

    sockaddr_in acceptSock;
    bzero((char*) &acceptSock, sizeof(acceptSock));  // zero out the data structure
    acceptSock.sin_family = AF_INET;   // using IP
    acceptSock.sin_addr.s_addr = htonl(INADDR_ANY); // listen on any address this computer has
    acceptSock.sin_port = htons(server_port);  // set the port to listen on

    int serverSd = socket(AF_INET, SOCK_STREAM, 0); // creates a new socket for IP using TCP

    const int on = 1;

    setsockopt(serverSd, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(int));  // this lets us reuse the socket without waiting for hte OS to recycle it

    // Bind the socket

    bind(serverSd, (sockaddr*) &acceptSock, sizeof(acceptSock));  // bind the socket using the parameters we set earlier
    
    // Listen on the socket
    int n = 5;
    listen(serverSd, n);  // listen on the socket and allow up to n connections to wait.

    std::vector<pthread_t> threadHolder;
    int i = 0;
    while(true)
    {
        sockaddr_in newsock;   // place to store parameters for the new connection
        socklen_t newsockSize = sizeof(newsock);
        int newSd = accept4(serverSd, (sockaddr *)&newsock, &newsockSize, SOCK_NONBLOCK);  // grabs the new connection and assigns it a temporary socket
        //If threadHolder contains threads, then try to join as many as possible in order to keep the number of open threads low
        //This also helps to make sure disconnected clients aren't taking up space in the server
        if(threadHolder.size() > 0)
        {
            i = 0;
            while(i < threadHolder.size())
            {
                int testJoin = pthread_tryjoin_np(threadHolder[i], NULL);
                if(testJoin == 0)
                {
                    std::cout << "Erasing thread" << std::endl;
                    threadHolder.erase(threadHolder.begin() + i);
                }
                else
                {
                    i++;
                }
            }
            i = threadHolder.size();
        }
        if(newSd == -1)
        {
            continue;
        }
        pthread_t tid;
        pthread_create(&tid, NULL, serviceConnection, (void*)&newSd);
        threadHolder.push_back(tid);
        i++;
        std::cout << threadHolder.size() << std::endl;
    }
    
    return 0;
}

//Legacy code for basic reading messages from TCP connected client socket
// void* threadConnection(void* newSd)
// {
//     int fd = *(int*)newSd;
//
//     while(true)
//     { 
//         // Read data from the socket
//         char buf[BUF_SIZE];
//         int bytesRead = read(fd, buf, BUF_SIZE);
//         if(bytesRead == 0 || bytesRead == -1)
//         {
//             continue;
//         }
//         pthread_mutex_lock(&lock);
//         std::cout << "read " << bytesRead << " bytes" << std::endl;
//         std::cout << "Received: " << buf << std::endl;
//         if(strcmp(buf, "exit") == 0 || strcmp(buf, "exit\r\n") == 0)
//         {
//             std::cout << "Closing connection" << std::endl;
//             close(fd);
//             pthread_mutex_unlock(&lock);
//             break;
//         }
//         else if(strcmp(buf, "HELP\r\n") == 0 || strcmp(buf, "HELP") == 0)
//         {
//             std::cout << "Port scanner possibly found." << std::endl;
//             write(fd, "Piss Off\n", 10);
//         }
//         memset(buf, 0, sizeof(buf));
//         pthread_mutex_unlock(&lock);
//     }
//    
//     return NULL;
// }

//Servicing TCP client connection.
void* serviceConnection(void* newSd)
{
    int fd = *(int*) newSd;
    player* p;
    char buf[BUF_SIZE];
    timeval tv;
    gettimeofday(&tv, NULL);
    timeval lap;
    while(true)
    {
        int bytesRead = read(fd, buf, BUF_SIZE);
        if(p != NULL && p->isInGame())//When the player is in game the logic will jump immediately to checking what is in the buffer
        {
            gettimeofday(&lap, NULL);
            if(bytesRead == 0 || bytesRead == -1)
            {
                goto label2;
            }
            goto label1;
        }
        if(bytesRead == 0 || bytesRead == -1)
        {
            gettimeofday(&lap, NULL);
            if(lap.tv_sec - tv.tv_sec > 300)
            {
                std::cout << "Connection timed out. Closing connection." << std::endl;
                if(p != NULL && p->isInLobby())
                {
                    std::cout << "Cleaning up player out of lobby" << std::endl;
                    lobby* l = p->getMyLobby();
                    if(l->popPlayer(p) != NULL)
                    {
                        updateNumLobbyPlayers(l);
                    }
                }
                close(fd);
                delete p;
                return NULL;
            }
        }
        label1:
        switch(buf[0])
        {
            case 'R':
                gettimeofday(&tv, NULL);
                std::cout << "Registering Player" << std::endl;
                p = registration(buf, fd);
                break;
            case 'L':
                gettimeofday(&tv, NULL);
                listLobbies(p);
                break;
            case 'C':
                gettimeofday(&tv, NULL);
                createLobby(buf, p);
                break;
            case 'I':
                gettimeofday(&tv, NULL);
                lobbyInfo(buf, p);
                break;
            case 'J':
                gettimeofday(&tv, NULL);
                joinLobby(buf, p);
                break;
            case 'E':
                gettimeofday(&tv, NULL);
                exitLobby(p);
                break;
            case 'S':
                gettimeofday(&tv, NULL);
                startGame(buf, p);
                break;
            case 'T':
                gettimeofday(&tv, NULL);
                changeDirection(buf, p);
                break;
            case 'D':
                gettimeofday(&tv, NULL);
                death(buf, p);
                break;
            case 'Q':
                std::cout << "Connection Closing" << std::endl;
                if(p != NULL && p->isInLobby())
                {
                    std::cout << "Cleaning up player out of lobby" << std::endl;
                    lobby* l = p->getMyLobby();
                    if(l->popPlayer(p) != NULL)
                    {
                        updateNumLobbyPlayers(l);
                    }
                }
                close(fd);
                delete p;
                return NULL;
        }
        label2:
        memset(buf, '\0', sizeof(buf));
        if(p != NULL && p->isInGame())//Sends game state information to all players in the lobby every 65000 micro seconds
        {
            pthread_mutex_lock(&lock);
            lobby* l = p->getMyLobby();
            if(((lap.tv_sec * 1000000) + lap.tv_usec) - ((l->lobbyTV.tv_sec * 1000000) + l->lobbyTV.tv_usec) > 65000)
            {
                gettimeofday(&l->lobbyTV, NULL);
                l->updatePlayersPos();
                std::string message;
                message += l->getAlivePlayersPos();
                for(player* indP: l->getPlayers())
                {
                    write(indP->getPlayerSocket(), message.c_str(), message.length());
                }
                std::cout << message << std::endl;
            }
            pthread_mutex_unlock(&lock);
        }
    }
    return NULL;
}

void addLobby(lobby* l)//Adds a lobby to the list of lobbies
{
    lobbies.push_back(l);
}

player* registration(char* buf, int& fd)//Registers a player to the game. Must do this in order to access other functionality
{
    player* p;
    std::string message;
    char* token;
    token = strtok(buf, "/"); //Grabs the fist letter of the command
    token = strtok(NULL, "\r\n"); //Grabs the name of the player to register
    if(token == NULL)
    {
        std::cout << "Null name space" << std::endl;
        message = "RF\n";
        write(fd, message.c_str(), message.length());
        return NULL;
    }
    if(strlen(token) < 3 || strlen(token) > 14)//Name is too long
    {
        std::cout << "Failed to register player " << token << " " << strlen(token) << std::endl;
        message = "RF\n";
        write(fd, message.c_str(), message.length());
        return NULL;
    }
    std::cout << token << std::endl;
    p = new player();//Create the player object and fill the proper values
    p->setName(token);
    p->setPlayerId(PLAYER_ID);
    PLAYER_ID++;
    p->setSocket(fd);
    message = "RT/";
    message += std::to_string(p->getPlayerId());
    message += "\n";
    write(p->getPlayerSocket(), message.c_str(), message.length());
    return p;
}

void listLobbies(player* p)//Lists all lobbies in the game
{
    if(p == NULL)
    {
        std::cout << "Client connection is trying to access lobby services before registration" << std::endl;
        return;
    }
    std::string message = "LT/";//Create the message to be sent back to the client
    for(lobby* l: lobbies)
    {
        bool isInGame = l->getAlivePlayersPos().size() > 0; //If there are no players in game, then this returns false which means that a game hasn't started yet
        message.append(std::to_string(l->getLobbyId()));
        message += "/";
        message.append(l->getLobbyName());
        message += "/";
        message.append(std::to_string(l->getLobbyNumPlayers()));
        message += "/";
        message.append(std::to_string(l->getLobbySize()));
        message += "/";
        message.append(std::to_string(isInGame));
        message.append("\n");
    }
    write(p->getPlayerSocket(), message.c_str(), message.length());
    return;
}

void createLobby(char* buf, player* p)//Creates a new lobby and adds the player to that lobby
{
    if(p == NULL)
    {
        std::cout << "Client connection is trying to access lobby services before registration" << std::endl;
        return;
    }
    if(p->isInLobby())//Pull player out of current lobby
    {
        lobby* l = p->getMyLobby();
        if(l->popPlayer(p) != NULL)
        {
            updateNumLobbyPlayers(l);
        }
    }
    p->setInGame(false);
    p->setAlive(true);
    std::string message;
    char* token = strtok(buf, "/"); //Grabs the first letter of the command
    token = strtok(NULL, "/"); //Grabs the lobby name
    if(token == NULL)
    {
        std::cout << "Name token failed" << std::endl;
        message = "CF\n";
        write(p->getPlayerSocket(), message.c_str(), message.length());
        return;
    }
    std::cout << token << std::endl;
    std::string name = token;
    token = strtok(NULL, "/");//grabs the size of the lobby to be created
    if(token == NULL)
    {
        std::cout << "Size token failed" << std::endl;
        message = "CF\n";
        write(p->getPlayerSocket(), message.c_str(), message.length());
        return;
    }
    int size = atoi(token);
    if(size <= 1)//Size too small
    {
        std::cout << "Cannot create a looby of size 1 or less" << std::endl;
        message = "CF\n";
        write(p->getPlayerSocket(), message.c_str(), message.length());
        return;
    }
    lobby* l = new lobby();//Create lobby object and fill the proper values
    l->setLobbyName(name);
    l->setLobbySize(size);
    l->setLobbyId(lobbyIDGen());
    l->addPlayer(p);
    lobbies.push_back(l);
    message = "CT/";//Message to send back to the client
    message += std::to_string(l->getLobbyId());
    message += "\n";
    write(p->getPlayerSocket(), message.c_str(), message.length());
    updateNumLobbyPlayers(l);
    return;
}

void lobbyInfo(char* buf, player* p)//Sends an information message to the specified player about the player ID's and player name's
{
    if(p == NULL)//If the player object doesn't exist yet
    {
        std::cout << "Client trying to access lobby services before registering" << std::endl;
        return;
    }
    std::string message;
    if(!p->isInLobby())//If the player isn't in a lobby yet then info cannot be sent about a lobby
    {
        std::cout << "Cannot access lobby info when player is not in a lobby" << std::endl;
        message = "IF\n";
        write(p->getPlayerSocket(), message.c_str(), message.length());
        return;
    }
    char* token = strtok(buf, "/");
    token = strtok(NULL, "/");
    if(token == NULL)//If the message is poorly formatted then this fails
    {
        std::cout << "lobby ID token failed" << std::endl;
        message = "IF\n";
        write(p->getPlayerSocket(), message.c_str(), message.length());
        return;
    }
    lobby* l = p->getMyLobby();
    if(l->getLobbyId() != atoi(token))//Message lobby id and the actual lobby id don't match
    {
        std::cout << "Player tried to get some other lobbies info that they aren't a part of" << std::endl;
        message = "IF\n";
        write(p->getPlayerSocket(), message.c_str(), message.length());
        return;
    }
    message = "IT/"; //Create the message to be sent back to the player
    for(player* indP: l->getPlayers())
    {
        message += std::to_string(p->getPlayerId());
        message += "/";
        message += p->getPlayerName();
        message += "\n";
    }
    std::cout << message << std::endl;
    write(p->getPlayerSocket(), message.c_str(), message.length());
}

void joinLobby(char* buf, player* p)//Allows the player to join lobbies that aren't full
{
    if(p == NULL)
    {
        std::cout << "Client trying to access lobby services before registration" << std::endl;
        return;
    }
    if(p->isInLobby())//Take player out of current lobby
    {
        lobby* l = p->getMyLobby();
        if(l->popPlayer(p) != NULL)
        {
            updateNumLobbyPlayers(l);
        }
    }
    p->setInGame(false);
    p->setAlive(true);
    std::string message;
    char* token = strtok(buf, "/");//Get the first letter of the command
    token = strtok(NULL, "\r\n");//Get the lobby ID
    if(token == NULL)
    {
        std::cout << "Missing lobby ID information" << std::endl;
        message = "JF\n";
        write(p->getPlayerSocket(), message.c_str(), message.length());
        return;
    }
    for(lobby* l: lobbies)//For every lobby on the server find the correct one and add the player to the lobby
    {
        if(l->getLobbyId() == atoi(token))
        {
            if(l->getLobbyNumPlayers() + 1 > l->getLobbySize())//Check to make sure the lobby isn't full
            {
                std::cout << "Cannot join a lobby that is full" << std::endl;
                message = "JF\n";
                write(p->getPlayerSocket(), message.c_str(), message.length());
                return;
            }
            if (l->getAlivePlayersPos().size() > 0)//Cannot join a lobby where a game has already started
            {
                std::cout << "Cannot join a lobby that is playing" << std::endl;
                message = "JF/\n";
                write(p->getPlayerSocket(), message.c_str(), message.length());
                return;
            }

            l->addPlayer(p);
            break;
        }
    }
    if(!p->isInLobby())//If the player wasn't added to a lobby then the lobby doesn't exist
    {
        std::cout << "Player tried to join a lobby that doesn exist" << std::endl;
        message = "JF\n";
        write(p->getPlayerSocket(), message.c_str(), message.length());
        return;
    }
    message = "JT\n";//Message to send back to client
    write(p->getPlayerSocket(), message.c_str(), message.length());
    updateNumLobbyPlayers(p->getMyLobby());
}

void exitLobby(player* p)//Allows the player to exit a lobby only if they occupy a lobby
{
    if(p == NULL)
    {
        std::cout << "Client trying to access lobby services before registering" << std::endl;
        return;
    }
    std::string message;
    if(!p->isInLobby())//Cannot exit a lobby when not in one
    {
        std::cout << "Player trying to exit a lobby while not in a lobby" << std::endl;
        message = "EF\n";
        write(p->getPlayerSocket(), message.c_str(), message.length());
        return;
    }
    lobby* l = p->getMyLobby();
    if(l->popPlayer(p) != NULL)
    {
        updateNumLobbyPlayers(l);//update the number of players that are in the lobby
    }
    p->setInGame(false);
    p->setAlive(true);
    message = "ET\n";//Message to send back to client
    write(p->getPlayerSocket(), message.c_str(), message.length());
}

void startGame(char* buf, player* p)//Starts a game for all players in the lobby the host is in. Give all players random position values.
{
    if(p == NULL)
    {
        std::cout << "Client trying to access game services before registering" << std::endl;
        return;
    }
    std::string message;
    char* token = strtok(buf, "/");
    token = strtok(NULL, "\r\n");
    if(token == NULL)//No lobby id was found in the message
    {
        std::cout << "LobbyId token failed" << std::endl;
        message = "SF\n";
        write(p->getPlayerSocket(), message.c_str(), message.length());
        return;
    }
    lobby* l = p->getMyLobby();
    if(l->getLobbyId() != atoi(token))//lobby id doesn't match the message token
    {
        std::cout << "Player trying to start a game from a lobby they aren't in" << std::endl;
        message = "SF\n";
        write(p->getPlayerSocket(), message.c_str(), message.length());
        return;
    }
    if(!l->findPlayer(p))//Player isn't in the lobby that it is trying to start a game from
    {
        std::cout << "Player trying to start a game from a lobby they aren't in" << std::endl;
        message = "SF\n";
        write(p->getPlayerSocket(), message.c_str(), message.length());
        return;
    }
    message = "ST\n";//Create message to send back to all the players
    for(player* indP: l->getPlayers())
    {
        std::cout << indP->getPlayerName() << std::endl;
        indP->setInGame(true);
        indP->setAlive(true);
        indP->setXPos(std::rand() % 50);
        indP->setYPos(std::rand() % 50);
        write(indP->getPlayerSocket(), message.c_str(), message.length());
    }
}

void changeDirection(char* buf, player* p)//Allows the player to change their direction in game
{
    std::cout << buf << std::endl;
    if(p == NULL)
    {
        std::cout << "Client trying to access game services before registering" << std::endl;
        return;
    }
    std::string message;
    if(!p->isInGame())//Player isn't in game and therefore shouldn't change direction
    {
        std::cout << "Player is trying to access game services while not in game" << std::endl;
        message = "TF\n";
        write(p->getPlayerSocket(), message.c_str(), message.length());
        return;
    }
    char* token = strtok(buf, "/");
    token = strtok(NULL, "\r\n");
    if(token == NULL)//No direction was given
    {
        std::cout << "Direction token failed" << std::endl;
        message = "TF\n";
        write(p->getPlayerSocket(), message.c_str(), message.length());
        return;
    }
    if(atoi(token) > 3 || atoi(token) < 0)//Direction given doesn't fit the proper format
    {
        std::cout << "Player sent invalid direction value" << std::endl;
        message = "TF\n";
        write(p->getPlayerSocket(), message.c_str(), message.length());
        return;
    }
    if(p->getDirection() == atoi(token))//Direction sent is the same as the way the player is already facing
    {
        return;
    }
    p->setDirection(atoi(token));
}

void death(char* buf, player* p)//Player dies and a winner is checked for
{
    if(p == NULL)
    {
        std::cout << "Client trying to access game services before registering" << std::endl;
        return;
    }
    std::string message;
    if(!p->isInGame())//Player isn't in a game so it shouldn't die
    {
        std::cout << "Player is trying to access game service while not in game" << std::endl;
        message = "DF\n";
        write(p->getPlayerSocket(), message.c_str(), message.length());
        return;
    }
    message = "DT\n";//Message to send back to client
    write(p->getPlayerSocket(), message.c_str(), message.length());
    p->setAlive(false);
    lobby* l = p->getMyLobby();
    if(l->checkForWinner())//A winner is check for
    {
        std::cout << "Game has concluded with a winner" << std::endl;
        std::string message = "W/";
        player* theWinner = l->getWinner();
        if(theWinner != NULL)//A winner was found
        {
            message += theWinner->getPlayerName(); //The name of the winning player is sent to every player in the lobby
            message += "\n";
            for(player* indP: l->getPlayers())
            {
                indP->setInGame(false);
                indP->setAlive(true);
                write(indP->getPlayerSocket(), message.c_str(), message.length());
            }
            return;
        }
        for(player* indP: l->getPlayers())
        {
            indP->setInGame(false);
            indP->setAlive(true);
        }
    }
}

void updateNumLobbyPlayers(lobby* l)//Sends an information message to all players noting who is in the lobby
{
    std::string message = "IT/";//Create message to send to all clients in the lobby
    for(player* p: l->getPlayers())
    {
        message += std::to_string(p->getPlayerId());
        message += "/";
        message += p->getPlayerName();
        message += "\n";
    }
    for(player* p: l->getPlayers())
    {
        write(p->getPlayerSocket(), message.c_str(), message.length());
    }
}

int lobbyIDGen()//Creates random integer from 3 to 99 which is unique to the lobby
{
    bool isUnique = false;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> randomInt(3,99);
    label:
    int lobbyIDVal = randomInt(gen);
    while(!isUnique)//Check to make sure no other lobby has the same ID
    {
        for(lobby* l: lobbies)
        {
            if(l->getLobbyId() == lobbyIDVal)
            {
                goto label;
            }
        }
        isUnique = true;
    }
    return lobbyIDVal;
}

