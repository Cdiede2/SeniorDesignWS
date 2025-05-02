#include <iostream>
#include <cstring>
#include <format>
#include <nlohmann/json.hpp>

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
 *      *   TODO: Possibly use threading to support multiple connections simultaneously
 *      *   TODO: Listen for TCP Connection on port 39554, initiating connection
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

    RECV_STAGE,         // Receive Request from client
    REQ_STAGE,          // Send Image
    RST_STAGE           // Reset Stage
};

class Server {
    public:
        // Constructors
        Server() : serverPort(39554), state() {};
        Server( int port ) : serverPort(port) {};

        // Mutators
        void setServerPort( int port );
        void setupServer();


    private:
        uint16_t serverPort;
        uint8_t state;

        struct sockaddr_in serverAddress;
};

void Server::setServerPort( int port ) {
    // Check that port is valid
    if( !( port > 0 && port < 65536 ) ) {
        throw std::exception();
    } 

    this->serverPort = static_cast<uint16_t>(port);
    return;
}

void Server::setupServer() {
    // Validate stage
    if ( this->state != IDLE_STAGE ) {
        throw std::exception();
    }


    this->serverAddress = {
        AF_INET,
        htons( this->serverPort ),
        INADDR_ANY
    };

}


int main(int argc, char **argv)
{
    const char *server_ip = "127.0.0.1";
    const int server_port = 39554;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr{
        AF_INET,
        htons(server_port),
        INADDR_ANY
    };

    std::cout << std::format("Server Listening On: {}:{}", server_ip, server_port) << std::endl;
    try
    {
        bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
        
        // Server Listening Loop
        while ( true ) {
            int client_socket;
            char buffer[1024] = {0};
            std::vector<std::string> header_vector;

            // Listen for incoming connections, supports maximum of 5 clients
            listen(sockfd, 5);
            client_socket = accept(sockfd, nullptr, nullptr);
            
            // Wait for Client to Send Data
            recv(client_socket, buffer, sizeof(buffer), 0);

            header_vector = split({buffer}, '/');

            // Check if Header is Valid
            if ( header_vector.size() != 3 || header_vector.at(0) != "0" )
            {
                std::cerr << std::format("ERROR: Invalid Header Received") << std::endl;
                close(client_socket);
                continue;
            } 
            
            // Check if Start bit is HIGH
            if( header_vector.at(1) == "80"  ) {
                std::cout << std::format("I received {}", reinterpret_cast<const char *>(buffer)) << std::endl;

                // Split and Read Values Sent by Client
                std::cout << "Index 0: " << header_vector.at(0) << std::endl;
                std::cout << "Index 1: " << header_vector.at(1) << std::endl;
                std::cout << "Index 2: " << header_vector.at(2) << std::endl;
            }
            ShannonFano sf;
            std::map<char, double> frequencies;
            std::map<char, std::string> codes;
            sf.buildCodes(frequencies, "hello");


            nlohmann::json json_data = sf.getCodes();
            send(client_socket, json_data.dump().c_str(), json_data.dump().size(), 0);


            // Check if Data Start bit is HIGH
            
            std::cout << "---------------------------" << std::endl;
            send(client_socket, "hello", 6, 0);
            close(client_socket);
        }





        // std::cout << std::format("Client Sent: {}", buffer) << std::endl;
    }
    catch (std::exception &err)
    {
        std::cerr << std::format("ERROR: {}", err.what()) << std::endl;
        return RETURN_NETWORK_ERR;
    }

    close(sockfd);
    return RETURN_OK;
}