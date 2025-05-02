#include <iostream>
#include <cstring>
#include <format>
#include <nlohmann/json.hpp>

#include <thread>
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <opencv4/opencv2/highgui.hpp>
// #include <openssl/md5.h>

// Networking
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

// #include "shannon-fano.h"
#include "camera.h" // Split and Strip functions
#include "shannon-fano.h" // Shannon-Fano Encoding

enum numbers
{
    RETURN_OK,
    RETURN_NETWORK_ERR
};

/** TODO List: Server
 *  TODO: Create Finite State Machine to keep track of which stage application is in
 *      *   TODO: Listening Stage
 *      *   TODO: Transmitting Stage
 *      *   TODO: Reset Stage
 *
 *  TODO: Listening Stage
 *      *   DONE: Possibly use threading to support multiple connections simultaneously
 *      *   DONE: Listen for TCP Connection on port 39554, initiating connection
 *      *       * TODO: Client connected, send HEADER + message indicating as such
 *      *       * TODO: Trigger Client to enter Listening Stage Using Listen flag
 *
 *  TODO: Transmitting Stage
 *      *   TODO: Read single image in from attached camera, store in VAR
 *      *   TODO: Process VAR
 *      *       * TODO: Apply Bilateral Gaussian Filter to VAR to create PROC_VAR
 *      *       * TODO: Apply MD5 Hashing Function to PROC_VAR, store in GLOBAL_HASH
 *      *       * TODO: Send Global Hash
 *      *       * TODO: Use 'color matrices' to "filter" image intro respective frames
 *      *       *       * TODO: Compute FRAME_N Hash
 *      *       *       * TODO: Compute FRAME_N SF_Table, Send over TCP with flag
 *      *       *       * TODO: Transmit hash and SF encoded FRAME_N over UDP
 *      *       *       * TODO: Repeat for each in N frames
 *
 *  TODO: Reset Stage
 *      *   TODO:
 */

 enum state{ 
    IDLE_STAGE,         // Idle Stage, wait for request from client
    LSTN_STAGE,         // Server is Listening for active connections
    RDY_STAGE,
    REQ_STAGE,          // Send Image
    RST_STAGE           // Reset Stage
};

class Server {
    public:
        // Constructors
        Server() : serverPort(39554), state(IDLE_STAGE), serverSocket( socket(AF_INET, SOCK_STREAM, 0) ) {};
        Server( int port ) : serverPort(port), state(IDLE_STAGE), serverSocket( socket(AF_INET, SOCK_STREAM, 0)) {};

        // Mutators
        void setServerPort( int port );
        void setupServer();

        // Listening Loop
        void serverLoop();

    private:
        uint16_t serverPort;
        uint8_t state;
        int serverSocket;
        struct sockaddr_in serverAddress;

        // Private Client Handle
        void client_handle( int client_socket );
};

/**
 * 
 */
void Server::setServerPort( int port ) {
    // Check that port is valid
    if( !( port > 0 && port < 65536 ) ) {
        throw std::exception();
    } 

    this->serverPort = static_cast<uint16_t>(port);
    return;
}

/**
 * 
 */
void Server::setupServer() {
    // Validate stage
    if ( this->state != IDLE_STAGE ) {
        throw std::runtime_error("Server Still in Idle");
    }

    // Seup Server Port
    this->serverAddress = {
        AF_INET,
        htons( this->serverPort ),
        INADDR_ANY
    };
    bind( this->serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    this->state = RDY_STAGE;
    return;
}

/**
 * 
 */
void Server::serverLoop() {
    if( this->state != RDY_STAGE ) {
        throw std::runtime_error("Server is not in the ready state to start the loop.");
    }

    // Main Server Loop
    for(;;) {
        int clientSocket;
        char buffer[1024] = {0};
        
        // Listen for Incoming Connections
        listen(serverSocket, 5);
        clientSocket = accept(serverSocket, nullptr, nullptr);
        
        // Read an image from the camera or file
        cv::Mat image = cv::imread("../image.jpg", cv::IMREAD_COLOR);
        if (image.empty()) {
            std::cerr << "Error: Could not read the image." << std::endl;
            close(clientSocket);
            continue;
        }

        // Display the image in a window
        cv::imshow("Server Image Display", image);
        cv::waitKey(1); // Allow the window to refresh

        std::thread thr(&Server::client_handle, this, clientSocket);
        thr.detach(); // Detach the thread to allow it to run independently
    }
    return;
}

void Server::client_handle( int client_socket ) {
    char buffer[1024] = {0};
    recv(client_socket, buffer, sizeof(buffer), 0);
    std::cout << std::format("On Socket {} I heard {}", client_socket, buffer) << std::endl;
    close(client_socket);
}


int main(int argc, char **argv)
{
    Server helloServer;
    helloServer.setServerPort(39554);
    helloServer.setupServer();
    try {
        helloServer.serverLoop();
    } catch( std::exception& exc ) {
        std::cerr << exc.what() << std::endl;
    }
    return RETURN_OK;
}