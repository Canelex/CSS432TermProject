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
#include "lobby.h"
//using namespace std;

const unsigned int BUF_SIZE = 65535;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

std::vector<lobby*> lobbies;

void* threadConnection(void* newSd);

void* serviceConnection(void* newSd);

void addLobby(lobby* l);

player* registration(char* buf, int& fd);

void listLobbies(player* p);

void createLobby(char* buf, player* p);

void lobbyInfo(char* buf, player* p);

void joinLobby(char* buf, player* p);

void exitLobby(char* buf, player* p);

void playerInLobbyCheck(player* p);

void updateNumLobbyPlayers(lobby* l);

int main () 
{
    lobby* lobby0 = new lobby();
    lobby* lobby1 = new lobby();
    lobby* lobby2 = new lobby();
    lobby0->setLobbySize(2);
    lobby0->setLobbyName("Preset 1 v 1 Match");
    lobby0->setLobbyId(0);
    lobby1->setLobbySize(3);
    lobby1->setLobbyName("Preset 1 v 1 v 1 Match");
    lobby1->setLobbyId(1);
    lobby2->setLobbySize(4);
    lobby2->setLobbyName("Preset 1 v 1 v 1 v 1 Match");
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
        pthread_create(&tid, NULL, serviceConnection, (void*)&newSd); //need to figure out how to start a thread from a class memeber function
        threadHolder.push_back(tid);
        i++;
        std::cout << threadHolder.size() << std::endl;
    }
    
    
    return 0;
}

//Legacy code for basic reading messages from TCP connected client socket
void* threadConnection(void* newSd)
{
    int fd = *(int*)newSd;

    while(true)
    { 
        // Read data from the socket
        char buf[BUF_SIZE];
        int bytesRead = read(fd, buf, BUF_SIZE);
        if(bytesRead == 0 || bytesRead == -1)
        {
            continue;
        }
        pthread_mutex_lock(&lock);
        std::cout << "read " << bytesRead << " bytes" << std::endl;
        std::cout << "Received: " << buf << std::endl;
        if(strcmp(buf, "exit") == 0 || strcmp(buf, "exit\r\n") == 0)
        {
            std::cout << "Closing connection" << std::endl;
            close(fd);
            pthread_mutex_unlock(&lock);
            break;
        }
        else if(strcmp(buf, "HELP\r\n") == 0 || strcmp(buf, "HELP") == 0)
        {
            std::cout << "Port scanner possibly found." << std::endl;
            write(fd, "Piss Off\n", 10);
        }
        memset(buf, 0, sizeof(buf));
        pthread_mutex_unlock(&lock);
    }
    
    return NULL;
}

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
        if(bytesRead == 0 || bytesRead == -1)
        {
            gettimeofday(&lap, NULL);
            if(lap.tv_sec - tv.tv_sec > 300)
            {
                std::cout << "Connection timed out. Closing connection." << std::endl;
                memset(buf, '\0', BUF_SIZE);
                buf[0] = 'Q';
                write(fd, buf, sizeof('Q'));
                close(fd);
                delete p;
                return NULL;
            }
        }
        switch(buf[0])
        {
            case 'R':
                gettimeofday(&tv, NULL);
                if(p != NULL)
                {
                    std::cout << "Client trying to double register a player" << std::endl;
                    std::string message = "RF\n";
                    write(p->getPlayerSocket(), message.c_str(), message.length());
                    break;
                }
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
                exitLobby(buf, p);
                break;
            case 'Q':
                std::cout << "Connection Closing" << std::endl;
                if(p != NULL && p->isInLobby())
                {
                    std::cout << "Cleaning up player out of lobby" << std::endl;
                    for(lobby* l: lobbies)
                    {
                        if(l->findPlayerPop(p) != NULL)
                        {
                            updateNumLobbyPlayers(l);
                        }
                    }
                }
                close(fd);
                delete p;
                return NULL;
        }
        memset(buf, '\0', sizeof(buf));
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
    if(strlen(token) < 3 || strlen(token) > 14)
    {
        std::cout << "Failed to register player " << token << " " << strlen(token) << std::endl;
        message = "RF\n";
        write(fd, message.c_str(), message.length());
        return NULL;
    }
    std::cout << token << std::endl;
    p = new player();
    p->setName(token);
    p->setSocket(fd);
    message = "RT\n";
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
    std::string message = "L/";
    for(lobby* l: lobbies)
    {
        message.append(std::to_string(l->getLobbyId()));
        message += "/";
        message.append(l->getLobbyName());
        message += "/";
        message.append(std::to_string(l->getLobbyNumPlayers()));
        message += "/";
        message.append(std::to_string(l->getLobbySize()));
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
    playerInLobbyCheck(p);
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
    if(size <= 1)
    {
        std::cout << "Cannot create a looby of size 1 or less" << std::endl;
        message = "CF\n";
        write(p->getPlayerSocket(), message.c_str(), message.length());
        return;
    }
    lobby* l = new lobby();
    l->setLobbyName(name);
    l->setLobbySize(size);
    l->setLobbyId(20); //placeholder until can add code for random two digit integer
    l->addPlayer(p);
    p->setInLobby(true);
    lobbies.push_back(l);
    message = "CT/";
    message += std::to_string(l->getLobbyId());
    message += "\n";
    write(p->getPlayerSocket(), message.c_str(), message.length());
    updateNumLobbyPlayers(l);
    return;
}

void lobbyInfo(char* buf, player* p)//Sends an information message to the specified player about how many players are in their current lobby
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
    bool sent = false;
    for(lobby* indL: lobbies)//Find the correct lobby and make sure the player is in that lobby
    {
        if(indL->getLobbyId() == atoi(token) && indL->findPlayer(p))
        {
            sent = true;
            message = "IT/";
            message += std::to_string(indL->getLobbyNumPlayers());
            message += "\n";
            write(p->getPlayerSocket(), message.c_str(), message.length());
            break;
        }
    }
    if(!sent)//If the message wasn't sent then the lobby doesn't exist or the player isn't in the specified lobby
    {
        std::cout << "Player tried to get some other lobbies info that they aren't a part of" << std::endl;
        message = "IF\n";
        write(p->getPlayerSocket(), message.c_str(), message.length());
        return; 
    }  
}

void joinLobby(char* buf, player* p)//Allows the player to join lobbies that aren't full
{
    if(p == NULL)
    {
        std::cout << "Client trying to access lobby services before registration" << std::endl;
        return;
    }
    playerInLobbyCheck(p);
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
    lobby* theLobby;
    for(lobby* l: lobbies)//For every lobby on the server find the correct one and add the player to the lobby
    {
        if(l->getLobbyId() == atoi(token))
        {
            if(l->getLobbyNumPlayers() + 1 > l->getLobbySize())//Check to make sure the lobby isn't full
            {
                std::cout << "Cannot join a lobby that is full" << std::endl;
                message = "CF\n";
                write(p->getPlayerSocket(), message.c_str(), message.length());
                return;
            }
            l->addPlayer(p);
            //updateNumLobbyPlayers(l);
            p->setInLobby(true);
            theLobby = l;
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
    message = "JT\n";
    write(p->getPlayerSocket(), message.c_str(), message.length());
    updateNumLobbyPlayers(theLobby);
}

void exitLobby(char* buf, player* p)//Allows the player to exit a lobby only if they occupy a lobby
{
    if(p == NULL)
    {
        std::cout << "Client trying to access lobby services before registering" << std::endl;
    }
    std::string message;
    if(!p->isInLobby())
    {
        std::cout << "Player trying to exit a lobby while not in a looby" << std::endl;
        message = "EF\n";
        write(p->getPlayerSocket(), message.c_str(), message.length());
        return;
    }
    playerInLobbyCheck(p);
    message = "ET\n";
    write(p->getPlayerSocket(), message.c_str(), message.length());
}

void playerInLobbyCheck(player* p)//Check if the player is in a lobby and pull them out if they are
{
    if(p->isInLobby())
    {
        std::cout << "Player is already in a lobby, taking player out of current lobby." << std::endl;
        for(lobby* indL: lobbies)
        {
            if(indL->findPlayerPop(p) != NULL)
            {
                p->setInLobby(false);
                updateNumLobbyPlayers(indL);
                break;
            }
        }
    }
    else
    {
        std::cout << "Player isn't in lobby, cannot take them out of lobby." << std::endl;
    }
    return;
}

void updateNumLobbyPlayers(lobby* l)//Sends an information message to all players noting how many players are in the lobby
{
    std::string message;
    for(player* p: l->getPlayers())
    {
        message = "IT/";
        message += std::to_string(l->getLobbyNumPlayers()).c_str();
        message += "\n";
        write(p->getPlayerSocket(), message.c_str(), message.length());
    }
}

