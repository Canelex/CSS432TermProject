#include "netman.h"
#include <thread>

/**
* Asynchronous packet handler. Constantly reads from socket,
* parses packets, and adds them to the incoming packet queue.
*/
void packetHandler(const char* address, int port, NetMan& net) {

    // Initialize WINSOCK
    WSAData wsaData;
    WORD DllVersion = MAKEWORD(2, 1);
    if (WSAStartup(DllVersion, &wsaData) != 0) {
        std::cerr << "Failed to setup winsock" << std::endl;
        return;
    }

    // Resolve hostname
    HOSTENT* host = gethostbyname(address);
    if (host == nullptr) {
        std::cout << "Failed to resolve hostname" << std::endl;
        return;
    }

    // Create sockaddr struct
    SOCKADDR_IN sin;
    ZeroMemory(&sin, sizeof(sin));
    sin.sin_port = htons(port);
    sin.sin_family = AF_INET; // IPv4
    memcpy(&sin.sin_addr.S_un.S_addr, host->h_addr_list[0], sizeof(sin.sin_addr.S_un.S_addr));

    // Until the program ends
    while (true) {

        // Create Socket object
        SOCKET sock = socket(AF_INET, SOCK_STREAM, 0); // TCP socket
        if (sock < 0) {
            std::cout << "Failed to create socket" << std::endl;
            return;
        }

        // Try to connect to the server
        if (connect(sock, (sockaddr*)&sin, sizeof(sin)) != 0) {
            std::cout << "Failed to connect to server: " << WSAGetLastError() << std::endl;
            this_thread::sleep_for(chrono::seconds(3)); // retry in 3 seconds
            continue;
        }

        net.connected = true;
        cout << "Connected to server" << endl;
        
        unsigned long howMuchInBuffer = 0;
        while (true) {
            // See how much is in buffer
            ioctlsocket(sock, FIONREAD, &howMuchInBuffer);

            // We have a packet waiting
            if (howMuchInBuffer > 0) {
                // Read incoming packet
                char buffer[256];
                int bytes = recv(sock, buffer, sizeof(buffer), 0);

                // Connection is closed
                if (bytes == 0) {
                    cout << "Disconnected from socket. Reconnecting now.";
                    closesocket(sock);
                    net.connected = false;
                    break;
                }

                // Pass to game
                buffer[bytes] = 0;
                cout << "Rec " << buffer << endl;
                net.incoming.push_back(string(buffer));
            }

            // Are there packets to send?
            if (!net.outgoing.empty()) {
                // Dequeue front packet
                string p = net.outgoing.front();
                net.outgoing.erase(net.outgoing.begin());

                // Send the data
                send(sock, p.c_str(), p.length(), 0);
                cout << "Sent packet " << p << endl;

                // Quit game (it'd be nice to close connection first)
                if (p == "Q") {
                    cout << "Killing thread (quit)";
                    closesocket(sock);
                    net.connected = false;
                    net.incoming.push_back("close");
                    return; // no data to rad
                }
            }
        }
    }
}


/**
* Parameterized constructor that sets up socket and starts
* packet handler thread
*/
NetMan::NetMan(const char* address, int port) {
    // Not connected by default
    connected = false;

    // Create a thread
    thread t(&packetHandler, address, port, ref(*this));

    // Let the thread run on its own
    t.detach();
}

/**
* Destructor for network manager
*/
NetMan::~NetMan() {
    
}

/**
* Sends register packet
*/
void NetMan::sendRegister(string username) {
    outgoing.push_back("R/" + username);
}

/**
* Sends list lobbies packet
*/
void NetMan::sendListLobbies() {
    outgoing.push_back("L");
}

/**
* Sends create lobby packet
*/
void NetMan::sendCreateLobby(string name, int size) {
    // Build packet
    string packet = "C/";
    packet += name;
    packet += "/";
    packet += to_string(size);

    // Send it
    outgoing.push_back(packet);
}

/**
* Sends lobby info packet
*/
void NetMan::sendLobbyInfo(int lobbyId) {
    outgoing.push_back("I/" + to_string(lobbyId));
}

/**
* Sends join lobby packet
*/
void NetMan::sendJoinLobby(int lobbyId) {
    outgoing.push_back("J/" + to_string(lobbyId));
}

/**
* Sends exit lobby packet
*/
void NetMan::sendExitLobby() {
    outgoing.push_back("E");
}

/**
* Sends quit game lobby packet
*/
void NetMan::sendQuitGame() {
    outgoing.push_back("Q");
}

/**
* Returns whether the network manager is connected to
* the server cosket
*/
bool NetMan::isConnected() const {
    return connected;
}

/**
* Returns whether there is an incoming packet sitting in
* the queue
*/
bool NetMan::hasNextPacket() {
    return !incoming.empty();
}

/**
 * Dequeues the most recent incoming packet and returns it.
 */
string NetMan::poll() {
    if (incoming.empty()) {
        return "";
    }

    string front = incoming.front();
    incoming.erase(incoming.begin());
    return front;
}