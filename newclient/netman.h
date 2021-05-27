/**
* netman.h/cpp
* 5/26/2021
*
* This wrapper is used to connect to the server. It abstracts away
* a lot of the networking logic so the game can focus on other things.
*/
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#pragma once
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <string>
#include <iostream>
using namespace std;

class NetMan
{
public:
    NetMan();

    ~NetMan();

    void connectToServer();

    bool sendRegister(const string &username);

    bool getServerList();

private:
    SOCKET sock;
};

