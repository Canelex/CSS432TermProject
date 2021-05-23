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
using namespace std;

const unsigned int BUF_SIZE = 65535;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void* threadConnection(void* newSd);

int main () 
{
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

    pthread_t tid[5];
    int i = 0;
    while(true)
    {
        sockaddr_in newsock;   // place to store parameters for the new connection
        socklen_t newsockSize = sizeof(newsock);
        int newSd = accept4(serverSd, (sockaddr *)&newsock, &newsockSize, SOCK_NONBLOCK);  // grabs the new connection and assigns it a temporary socket
        if(newSd == -1)
        {
            continue;
        }
        pthread_create(&tid[i], NULL, threadConnection, (void*)&newSd);
        i++;
        if(i >= 5)
        {
            i = 0;
            while(i < 5)
            {
                pthread_join(tid[i], NULL);
                i++;
            }
            i = 0;
        }
    }
    
    
    return 0;
}

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
        cout << "read " << bytesRead << " bytes" << endl;
        cout << "Received: " << buf << endl;
        if(strcmp(buf, "exit") == 0)
        {
            cout << "Closing connection" << endl;
            close(fd);
            pthread_mutex_unlock(&lock);
            break;
        }
        memset(buf, 0, sizeof(buf));
        //buf = "message received";
        //write(*(int*)newSd, buf, sizeof(buf));
        pthread_mutex_unlock(&lock);
    }
    
    return NULL;
}