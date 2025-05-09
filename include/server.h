#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <cstring>
#include <format>
#include <cassert>
#include <nlohmann/json.hpp>
#include <openssl/evp.h>
#include <thread>
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "camera.h"
#include "shannon-fano.h"

/** TODO List: Server
 *  TODO: Create Finite State Machine to keep track of which stage application is in
 *      *   TODO: Listening Stage
 *      *   TODO: Transmitting Stage
 *      *   TODO: Reset Stage
 *
 *  DONE: Listening Stage
 *      *   DONE: Possibly use threading to support multiple connections simultaneously
 *      *   DONE: Listen for TCP Connection on port 39554, initiating connection
 *      *       * DONE: Client connected, send HEADER + message indicating as such
 *      *       * DONE: Trigger Client to enter Listening Stage Using Listen flag
 *
 *  TODO: Transmitting Stage
 *      *   INPROGRESS: Read single image in from attached camera, store in VAR
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


enum state
{
    IDLE_STAGE, // Idle Stage, wait for request from client
    LSTN_STAGE, // Server is Listening for active connections
    RDY_STAGE,
    REQ_STAGE, // Send Image
    RST_STAGE  // Reset Stage
};

class Server
{
public:
    // Constructors
    Server() : serverPort(39554), state(IDLE_STAGE), serverSocket(socket(AF_INET, SOCK_STREAM, 0)) {};
    Server(int port) : serverPort(port), state(IDLE_STAGE), serverSocket(socket(AF_INET, SOCK_STREAM, 0)) {};

    // Mutators
    void setListeningAddress( const std::string& );
    void setListeningPort( int port );

    void setServerPort(const char *mAddr, int port); // Setup the server Address and Port
    void setupServer();                              // Create listening socket
    cv::Mat getCameraFrame();                        // Access media and retrieve image

    // Accessors
    std::string getListeningAddress() const;
    int getListeningPort() const;

    // Listening Loop
    void serverLoop(); // Main server loop

private:
    std::string listenAddr;  // Listening address
    int serverPort;
    struct sockaddr_in server_sin;

    uint8_t state;

    int serverSocket;

    // Private Client Handle
    void client_handle(int client_socket); // Client thread
    bool imageProc(const cv::Mat &input, const std::vector<Filter> &filters, std::vector<std::pair<cv::Mat, std::string>> &ret_val);
    std::vector<Filter> buildFilterArray( nlohmann::json& request );
};
#endif