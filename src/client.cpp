#include <iostream>
#include <format>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "camera.h"

enum numbers
{
    RETURN_OK,
    RETURN_NETWORK_ERR,
    RETURN_USR_ERR
};

/** TODO List: Client
 *  TODO: Create Finite State Machine to keep track of which stage application is in
 *      *   TODO: Idle Stage
 *      *   TODO: Connect to server
 *      *   TODO: Retrieve Image
 *      *       * TODO: Request image
 *      *       * TODO: Receive image
 *      *       * TODO: Reconstruct image
 *      *       * TODO: Validate image
 *      *   TODO: Reset Stage
 * 
 *  INPROGRESS: Idle Stage, wait for call from user
 *  INPROGRESS: Connect to Server
 *  INPROGRESS: Retrieve Image
 *  INPROGRESS: Reset Stage
 */

enum state{ 
    IDLE_STAGE,         // Idle Stage, wait for call from user
    REQ_STAGE,          // Connect to Server
    RECV_STAGE,         // Retrieve Image
    RST_STAGE           // Reset Stage
};

class Client
{
    public:
        // Constructor
        Client() : serverSocket(39554), state(IDLE_STAGE), clientSocket(socket(AF_INET, SOCK_STREAM, 0)) {};
        Client(int port) : serverSocket(port), state(IDLE_STAGE), clientSocket(socket(AF_INET, SOCK_STREAM, 0)) {};

        // Mutators
        void setServerSocket( int socket );
        void connectToServer();
        void sendRequestSrv();
        void recvFromsrv();

        // Accessors
        uint8_t getCurrentState() const { return this->state; }
        int getCurrentClientSocket() const { return this->clientSocket; }
        int getCurrentServerSocket() const { return this->serverSocket; }

        
    private:
        uint8_t state;
        int serverSocket;
        int clientSocket;
        sockaddr_in serverAddr;
};

/**
 * @brief
 * @details
 * @param int socket
 */
void Client::setServerSocket( int socket ) {
    // Check Client is in IDLE state
    if( this->state != IDLE_STAGE ) {
        throw std::exception();
    }
    
    // Check socket is valid
    if( socket > 0 && socket < 65536 ) {
        this->serverSocket = socket;
    } else {
        throw std::exception();
    }
    return;
}

/**
 * 
 */
void Client::connectToServer() {
    // Check Client is in IDLE stage
    if( this->state != IDLE_STAGE ) {
        throw std::exception();
    }

    // Proceed with connection
    this->serverAddr.sin_family = AF_INET;
    this->serverAddr.sin_port = htons(this->serverSocket);

    if (inet_pton(AF_INET, "127.0.0.1", &this->serverAddr.sin_addr) <= 0) {
        throw std::exception();
    }
    if (connect(this->clientSocket, (struct sockaddr *)&this->serverAddr, sizeof(this->serverAddr)) < 0) {
        throw std::exception();
    }

    // Update state to REQ(UEST) STAGE, indactes client is ready to send request to server
    this->state = REQ_STAGE;
}

void Client::sendRequestSrv() {
    // Check Client is in REQ_STAGE
    if( this->state != REQ_STAGE ) {
        throw std::exception();
    }

    send(clientSocket, reinterpret_cast<const char*>("Hello Server"), 20, 0);
}


int main(int argc, char **argv)
{
    char* buffer[1024];
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr{
        AF_INET,
        htons(39554),
        INADDR_ANY};

    CamHeader header{
        0,
        0x80,
        64
    };

    std::cout << std::format("Client Connecting To: {}:{}", "127.0.0.1", 39554) << std::endl;
    try
    {
        std::string message = std::format("{}/{:02x}/{}", header.protocol, header.flags, header.size );

        connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
        
        // Send Header
        // send(clientSocket, reinterpret_cast<const char *>("Hello World!"), 13, 0);
        send(clientSocket, message.c_str(), message.size(), 0 );
        recv(clientSocket, buffer, sizeof(buffer), 0);
        std::cout << std::format("I received {}", reinterpret_cast<const char*>(buffer)) << std::endl;
        // std::cout << std::format("Val: {}", "hello") << std::endl;
        close(clientSocket);
    }
    catch (std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return RETURN_NETWORK_ERR;
    }

    std::cout << "Hello World!" << std::endl;
    return RETURN_OK;
}