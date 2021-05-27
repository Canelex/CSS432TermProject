
#include "netman.h"
#define PORT 12345

NetMan::NetMan() {

}

NetMan::~NetMan() {

}

void printHello(void* test) {

}

void NetMan::connectToServer() {
    // Init WINSOCK
    WSAData wsaData;
    WORD DllVersion = MAKEWORD(2, 1);
    if (WSAStartup(DllVersion, &wsaData) != 0) {
        std::cout << "Failed to setup winsock" << std::endl;
        return;
    }

    // Create Socket
    sock = socket(AF_INET, SOCK_STREAM, 0); // TCP socket
    if (sock < 0) {
        std::cout << "Failed to create socket" << std::endl;
        return;
    }

    // Get Server Info
    HOSTENT* host = gethostbyname("50.35.123.243");
    if (host == nullptr) {
        std::cout << "Failed to resolve hostname" << std::endl;
        return;
    }

    // Create sockaddr struct
    SOCKADDR_IN sin;
    ZeroMemory(&sin, sizeof(sin));
    sin.sin_port = htons(PORT);
    sin.sin_family = AF_INET; // IPv4
    memcpy(&sin.sin_addr.S_un.S_addr, host->h_addr_list[0], sizeof(sin.sin_addr.S_un.S_addr));

    // Connect to server
    if (connect(sock, (const sockaddr*)&sin, sizeof(sin)) != 0) {
        std::cout << "Failed to connect to server" << std::endl;
        std::cout << WSAGetLastError() << endl;
        return;
    }

    cout << "Connected to server" << endl;
}

bool NetMan::sendRegister(const string& username) {
    
    char buffer[1024];

    string msg = "R" + username;
    cout << "Sending " << msg << endl;
    send(sock, msg.c_str(), msg.size(), 0);
    int res = recv(sock, buffer, 1024, 0);

    cout << "Read " << res << " bytes." << endl;

    if (res < 0) {
        cout << "Failed to read response to register." << endl;
        return false;
    }

    buffer[res] = '\0';
    cout << "Registration response: " << buffer << endl;
    return true;
}

bool NetMan::getServerList() {
    char buffer[1024];

    string msg = "L";
    cout << "Sending " << msg << endl;
    send(sock, msg.c_str(), msg.size(), 0);
    int res = recv(sock, buffer, 1024, 0);

    cout << "Read " << res << " bytes." << endl;

    if (res < 0) {
        cout << "Failed to read response to serverlist." << endl;
        return false;
    }

    buffer[res] = '\0';
    cout << "Serverlist response: " << buffer << endl;
    return true;
}