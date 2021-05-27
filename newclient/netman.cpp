#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <iostream>
#include "netman.h"
#include <string>

#define PORT 80

NetMan::NetMan() {

}

NetMan::~NetMan() {

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
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0); // TCP socket
    if (sock < 0) {
        std::cout << "Failed to create socket" << std::endl;
        return;
    }

    // Get Server Info
    HOSTENT* host = gethostbyname("neverssl.com");
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
        return;
    }

    const char message[] = "R1234567891011\0";

    if (!send(sock, message, sizeof(message), 0)) {
        std::cout << "Failed to send message";
    }

    char msg[2048];
    char buffer[100];
    int bytes_read = 0;
    int offset = 0;

    std::string s = "";
    while ((bytes_read = recv(sock, buffer, 100, 0)) > 0) {
        std::cout << "Read bytes " << bytes_read << std::endl;
        memcpy(msg + offset, buffer, bytes_read);
        offset += bytes_read;
        msg[offset] = 0;
    }

    printf("%s\n", msg);
    closesocket(sock);

    std::cout << "Everything's good" << std::endl;
}

void NetMan::disconnect() {

}