#include "netman.h"
#include <thread>

/**
* Asynchronous packet handler. Constantly reads from socket,
* parses packets, and adds them to the incoming packet queue.
*/
void packetHandler(const char* address, int port, vector<string>* incoming, vector<string>* outgoing) {

    // Initialize WINSOCK
    WSAData wsaData;
    WORD DllVersion = MAKEWORD(2, 1);
    if (WSAStartup(DllVersion, &wsaData) != 0) {
        std::cerr << "Failed to setup winsock" << std::endl;
        return;
    }

    // Create Socket object
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0); // TCP socket
    if (sock < 0) {
        std::cout << "Failed to create socket" << std::endl;
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

        // Try to connect to the server
        if (connect(sock, (sockaddr*)&sin, sizeof(sin)) != 0) {
            std::cout << "Failed to connect to server: " << WSAGetLastError() << std::endl;
            this_thread::sleep_for(chrono::seconds(3)); // retry in 3 seconds
            continue;
        }

        cout << "Connected to server" << endl;
        

        // Success! Let's keep reading packets
        while (true) {
            // Are there packets to send?
            if (!outgoing->empty()) {
                // Dequeue front packet
                string p = outgoing->front();
                outgoing->erase(outgoing->begin());

                // Send the data
                send(sock, p.c_str(), p.length(), 0);
                cout << "Sent packet " << p << endl;

                // Read incoming packets
                char buffer[256];
                int bytes = recv(sock, buffer, sizeof(buffer), 0);
                buffer[bytes] = 0;
                incoming->push_back(string(buffer));
            }
        }
    }
}


/* Default constructor */
NetMan::NetMan() {

}

/**
* Parameterized constructor that sets up socket and starts
* packet handler thread
*/
NetMan::NetMan(const char* address, int port) {
    // Create queues
    incoming = new vector<string>();
    outgoing = new vector<string>();

    // Create a thread
    thread t(&packetHandler, address, port, incoming, outgoing);

    // Let the thread run on its own
    t.detach();
}

/**
* Destructor for network manager
*/
NetMan::~NetMan() {

}

/**
* Sends a packet to the server but does not wait for answer.
* Response will be picked up by the thread.
*/
void NetMan::dispatch(string packet) {
    outgoing->push_back(packet);
}

/**
* Returns whether there is an incoming packet sitting in
* the queue
*/
bool NetMan::hasNextPacket() {
    return !incoming->empty();
}

/**
 * Dequeues the most recent incoming packet and returns it.
 */
string NetMan::poll() {
    if (incoming->empty()) {
        return "";
    }

    string front = incoming->front();
    incoming->erase(incoming->begin());
    return front;
}