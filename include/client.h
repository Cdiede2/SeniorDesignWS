#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <format>
#include <nlohmann/json.hpp>
#include <openssl/evp.h>
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <array>
#include "camera.h"

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



enum state
{
    IDLE_STAGE, // Idle Stage, wait for call from user
    REQ_STAGE,  // Connect to Server
    RECV_STAGE, // Retrieve Image
    RST_STAGE   // Reset Stage
};

class Client
{
public:
    // Constructor
    Client() : serverAddr("255.255.255.255"), serverPort(39554), state(IDLE_STAGE), clientSocket(socket(AF_INET, SOCK_STREAM, 0)) {};
    Client(int port) : serverAddr("255.255.255.255"), serverPort(port), state(IDLE_STAGE), clientSocket(socket(AF_INET, SOCK_STREAM, 0)) {};

    // Deconstructor
    ~Client();
    
    // Mutators
    void connectToServer();

    void sendRequestSrv();
    std::vector<cv::Mat> recvFrames();
    
    void setServerPort(int socket);
    void setServerAddress(const std::string&);

    // Accessors
    int getCurrentClientSocket() const { return this->clientSocket; }
    int getCurrentServerSocket() const { return this->serverPort; }
    
    uint8_t getCurrentState() const { return this->state; }
    std::string getServerAddress() const;
    int getServerPort() const;

private:
    uint8_t state;
    int serverPort;
    int clientSocket;
    sockaddr_in server;
    std::string serverAddr;
};
#endif