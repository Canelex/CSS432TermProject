/**
* netman.h/cpp
* 5/26/2021
*
* This wrapper is used to connect to the server. It abstracts away
* a lot of the networking logic so the game can focus on other things.
*/
#pragma once

class NetMan
{
public:
    NetMan();
    ~NetMan();

    void connectToServer();

    void disconnect();

private:
};

