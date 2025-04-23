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

int main(int argc, char **argv)
{
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr{
        AF_INET,
        htons(39554),
        INADDR_ANY};

    std::cout << std::format("Client Connecting To: {}:{}", "127.0.0.1", 39554) << std::endl;
    try
    {
        connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
        send(clientSocket, reinterpret_cast<const char *>("Hello World!"), 13, 0);
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