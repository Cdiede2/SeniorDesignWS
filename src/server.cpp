#include <iostream>
#include <cstring>
#include <openssl/md5.h>
#include <format>

#include <iomanip>
#include <vector>
#include <map>

// #include <opencv4/opencv2/core.hpp>
// #include <opencv4/opencv2/imgproc.hpp>
// #include <opencv4/opencv2/highgui.hpp>


#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "shannon-fano.h"


// reinterpret_cast<int*>(0x00000000);

int main(int argc, char** argv) {
    const char* server_ip = "127.0.0.1";
    const int server_port = 39554;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr{
        AF_INET,
        htons(server_port),
        INADDR_ANY
    };
    
    std::cout << std::format("Server Listening On: {}:{}", server_ip, server_port) << std::endl;
    
    bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr) );
    listen(sockfd, 5);

    int client_socket = accept(sockfd, nullptr, nullptr);
    char buffer[1024] = {0};
    
    recv(client_socket, buffer, sizeof(buffer), 0);
    std::cout << std::format("Client Sent: {}", buffer) << std::endl;
    close(sockfd);
    return 0;
}