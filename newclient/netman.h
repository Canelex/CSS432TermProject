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
#include <vector>

using namespace std;

class NetMan
{
public:
    /* Default constructor */
    NetMan();

    /**
    * Parameterized constructor that sets up socket and starts
    * packet handler thread
    */
    NetMan(const char* address, int port);

    /**
    * Destructor for network manager
    */
    ~NetMan();

    /**
    * Sends a packet to the server but does not wait for answer.
    * Response will be picked up by the thread.
    */
    void dispatch(string packet);

    /** 
    * Returns whether there is an incoming packet sitting in 
    * the queue 
    */
    bool hasNextPacket();

    /**
     * Dequeues the most recent incoming packet and returns it.
     */
    string poll();

private:
    // List of packets to send
    vector<string>* outgoing;

    // List of packets received
    vector<string>* incoming;
};

