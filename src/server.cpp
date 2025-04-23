#include <iostream>
#include <cstring>
#include <format>

// #include <opencv4/opencv2/core.hpp>
// #include <opencv4/opencv2/imgproc.hpp>
// #include <opencv4/opencv2/highgui.hpp>
// #include <openssl/md5.h>

// Networking
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

// #include "shannon-fano.h"

// reinterpret_cast<int*>(0x00000000);

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
    const char *server_ip = "127.0.0.1";
    const int server_port = 39554;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr{
        AF_INET,
        htons(server_port),
        INADDR_ANY};

    std::cout << std::format("Server Listening On: {}:{}", server_ip, server_port) << std::endl;
    try
    {
        int client_socket;
        char buffer[1024] = {0};

        bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));

        // Listen for incoming connections, supports maximum of 5 clients
        listen(sockfd, 5);
        client_socket = accept(sockfd, nullptr, nullptr);

        recv(client_socket, buffer, sizeof(buffer), 0);
        std::cout << std::format("Client Sent: {}", buffer) << std::endl;

    }
    catch (std::exception &err)
    {
        std::cerr << std::format("ERROR: {}", err.what()) << std::endl;
        return RETURN_NETWORK_ERR;
    }

    close(sockfd);
    return RETURN_OK;
}