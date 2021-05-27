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

void createLobby(char* buf);

void lobbyInfo(char* buf);

void joinLobby(char* buf);

void exitLobby(char* buf);

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
        if(i >= 5)
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
            if(lap.tv_sec - tv.tv_sec > 10)
            {
                std::cout << "Connection timed out. Closing connection." << std::endl;
                close(fd);
                delete p;
                return NULL;
            }
        }
        switch(buf[0])
        {
            case 'R':
                gettimeofday(&tv, NULL);
                std::cout << buf << std::endl;
                std::cout << "Registering Player" << std::endl;
                p = registration(buf, fd);
                break;
            case 'L':
                gettimeofday(&tv, NULL);
                listLobbies(p);
                break;
            case 'C':
                gettimeofday(&tv, NULL);
                createLobby(buf);
                break;
            case 'I':
                gettimeofday(&tv, NULL);
                lobbyInfo(buf);
                break;
            case 'J':
                gettimeofday(&tv, NULL);
                joinLobby(buf);
                break;
            case 'E':
                gettimeofday(&tv, NULL);
                exitLobby(buf);
                break;
            case 'Q':
                std::cout << "Connection Closing" << std::endl;
                close(fd);
                delete p;
                return NULL;
        }
        memset(buf, '\0', sizeof(buf));
    }
    return NULL;
}

void addLobby(lobby* l)
{
    lobbies.push_back(l);
}

player* registration(char* buf, int& fd)
{
    player* p;
    std::string message;
    char* token;
    token = strtok(buf, "R");
    if(strlen(token) < 3 || strlen(token) > 10)
    {
        message = "RF\n";
        write(fd, message.c_str(), message.length());
        return NULL;
    }
    p = new player();
    p->setName(token);
    p->setSocket(fd);
    message = "RT\n";
    write(p->getPlayerSocket(), message.c_str(), message.length());
    return p;
}

void listLobbies(player* p)
{
    if(p == NULL)
    {
        std::cout << "Client connection is trying to access lobby services before registration" << std::endl;
        return;
    }
    std::string message = "L";
    for(lobby* l: lobbies)
    {
        message.append(std::to_string(l->getLobbyId()));
        message.append(l->getLobbyName());
        message.append(std::to_string(l->getLobbyNumPlayers()));
        message.append(std::to_string(l->getLobbySize()));
        message.append("\n");
    }
    write(p->getPlayerSocket(), message.c_str(), message.length());
    return;
}

void createLobby(char* buf)
{

}

void lobbyInfo(char* buf)
{

}

void joinLobby(char* buf)
{

}

void exitLobby(char* buf)
{

}

