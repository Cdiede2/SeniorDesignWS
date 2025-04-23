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
    RETURN_NETWORK_ERR
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
 *  TODO: Idle Stage, wait for call from user
 *  TODO: Connect to Server
 *  TODO: Retrieve Image
 *  TODO: Reset Stage
 */

int main(int argc, char **argv)
{
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr{
        AF_INET,
        htons(39554),
        INADDR_ANY};

    CamHeader header{
        0,
        0x00,
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