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
    * Sends register packet
    */
    void sendRegister(string username);

    /**
    * Sends list lobbies packet
    */
    void sendListLobbies();

    /**
    * Sends create lobby packet
    */
    void sendCreateLobby(string name, int size);

    /**
    * Sends lobby info packet
    */
    void sendLobbyInfo(int lobbyId);

    /**
    * Sends join lobby packet
    */
    void sendJoinLobby(int lobbyId);

    /**
    * Sends exit lobby packet
    */
    void sendExitLobby();

    /**
    * Sends quit game lobby packet
    */
    void sendQuitGame();

    /**
    * Sends start game packet
    */
    void sendStartGame(int id);

    /**
    * Sends updated player turn
    * 0 -> up
    * 1 -> right
    * 2 -> down
    * 3 -> left
    */
    void sendPlayerDir(int dir);

    /**
    * Sends player dead event
    */
    void sendPlayerDead();

    /**
    * Returns whether the network manager is connected to
    * the server cosket
    */
    bool isConnected() const;

    /** 
    * Returns whether there is an incoming packet sitting in 
    * the queue 
    */
    bool hasNextPacket();

    /**
     * Dequeues the most recent incoming packet and returns it.
     */
    string poll();

    /**
    * Asynchronous packet handler. Constantly reads from socket,
    * parses packets, and adds them to the incoming packet queue.
    */
    friend void packetHandler(const char* address, int port, NetMan& net);

    /**
    * Static helper for splitting packets
    */
    static vector<string> split(string source, string delim);

private:
    // Is the netman connected to server?
    bool connected;

    // List of packets to send
    vector<string> outgoing;

    // List of packets received
    vector<string> incoming;
};

