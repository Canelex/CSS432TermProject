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
using namespace std;

const unsigned int BUF_SIZE = 65535;

void serviceConnection(int newSd);

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

    sockaddr_in newsock;   // place to store parameters for the new connection
    socklen_t newsockSize = sizeof(newsock);
    int pid;
    while(true)
    {
        int newSd = accept(serverSd, (sockaddr *)&newsock, &newsockSize);  // grabs the new connection and assigns it a temporary socket
        if(newSd < 0)
        {
            cerr << "ERROR on accept" << endl;
        }
        pid = fork();
        if(pid < 0)
        {
            cerr << "ERROR on fork" << endl;
        }
        if(pid == 0)
        {
            close(serverSd);
            serviceConnection(newSd);
            exit(0);
        }
        else
        {
            close(newSd);
        }
    }
    close(serverSd);
    
    
    return 0;
}

void serviceConnection(int newSd)
{
    while(true)
    { 
        // Read data from the socket
        char buf[BUF_SIZE];
        int bytesRead = read(newSd, buf, BUF_SIZE);
        if(bytesRead == 0 || bytesRead == -1)
        {
            continue;
        }
        cout << "read " << bytesRead << " bytes" << endl;
        cout << "Received: " << buf << endl;
        if(strcmp(buf, "exit") == 0)
        {
            cout << "Closing connection" << endl;
            close(newSd);
            break;
        }
        memset(buf, 0, sizeof(buf));
        //buf = "message received";
        //write(*(int*)newSd, buf, sizeof(buf));
    }
    
    return;
}