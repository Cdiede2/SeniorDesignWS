#include <iostream>
#include <format>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

enum numbers
{
    RETURN_OK,
    RETURN_NETWORK_ERR
};

struct CamHeader {
    uint8_t protocol;
    uint8_t flags;  // (S)tart || (R)esponse || (E)nd || Unused...
    uint16_t size;
};

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