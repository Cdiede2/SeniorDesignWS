#include <iostream>
#include <format>






#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <opencv4/opencv2/highgui.hpp>

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

        // Deconstructor
        ~Client();

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

        void sendImage( const cv::Mat& image ){
            std::vector<uint8_t> buffer;
            // cv::imencode(".jpg", image, buffer);
            std::cout << "Sending Image" << std::endl;
        }

};

/**
 * 
 */
Client::~Client() {
    close( this->clientSocket );
}


/**
 * @brief Sets the server socket for the client.
 * 
 * This function assigns a valid socket to the client, provided the client
 * is in the IDLE state and the socket value is within the valid range.
 * 
 * @param socket The socket descriptor to be set for the server connection.
 *               Must be a positive integer within the range [1, 65535].
 * 
 * @throws std::exception If the client is not in the IDLE state.
 * @throws std::exception If the provided socket value is invalid (not in the range [1, 65535]).
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

    // Proceed with Connection 
    this->serverAddr = {
        AF_INET,
        htons(this->serverSocket),
        INADDR_ANY
    };

    if (connect(this->clientSocket, (struct sockaddr *)&this->serverAddr, sizeof(this->serverAddr)) < 0) {
        throw std::exception();
    }

    // Update state to REQ(UEST) STAGE, indactes client is ready to send request to server
    this->state = REQ_STAGE;
}

/**
 * 
 */
void Client::sendRequestSrv() {
    char buffer[1024] = {0};

    // Check Client is in REQ_STAGE
    if( this->state != REQ_STAGE ) {
        throw std::exception();
    }

    // Request Sent, wait for FIN response indicating last packet/frame
    std::cout << "Hello World" << std::endl;
    send(clientSocket, reinterpret_cast<const char*>("Hello Server"), 20, 0);

    recv(clientSocket, buffer, sizeof(buffer), 0);
    std::cout << std::format("\t==> {}", buffer) << std::endl;
}


int main(int argc, char **argv)
{
    Client hello_client;
    hello_client.setServerSocket(39554);
    hello_client.connectToServer();
    hello_client.sendRequestSrv();

    return RETURN_OK;
}