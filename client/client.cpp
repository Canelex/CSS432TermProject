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

int main(int argc, char** argv)
{
    if(argc != 2)
    {
        cout << "Wrong number of arguments. Should be 2." << endl;
        return 0;
    }
    // Create the socket
    int server_port = 12345;

    struct hostent* host = gethostbyname(argv[1]);

    sockaddr_in sendSockAddr;

    bzero((char*)&sendSockAddr, sizeof(sendSockAddr));

    sendSockAddr.sin_family = AF_INET;
    sendSockAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
    sendSockAddr.sin_port = htons(server_port);

    int clientSd = socket(AF_INET, SOCK_STREAM, 0);

    int connectStatus = connect(clientSd, (sockaddr*)&sendSockAddr, sizeof(sendSockAddr));
    if (connectStatus < 0)
    {
       cerr << "Failed to connect to the server" << endl;
       return 0;
    }

    while(true)
    {
        cout << "Please type message to send: ";
        string message;
        getline(cin, message);
        cout << message << endl;
        int x = write(clientSd, message.c_str(), message.length());
        if(x == -1)
        {
            cout << "Failed to write" << endl;
            continue;
        }
        cout << "Successful write" << endl;
        if(message == "exit")
        {
            cout << "Exiting client" << endl;
            break;
        }
    }
    return 0;
}