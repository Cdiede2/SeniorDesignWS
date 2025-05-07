#include <iostream>
#include <format>
#include <nlohmann/json.hpp>
// #include <openssl/md5.h>
#include <openssl/evp.h>

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
    Client() : serverSocket(39554), state(IDLE_STAGE), clientSocket(socket(AF_INET, SOCK_STREAM, 0)) {};
    Client(int port) : serverSocket(port), state(IDLE_STAGE), clientSocket(socket(AF_INET, SOCK_STREAM, 0)) {};

    // Deconstructor
    ~Client();

    // Mutators
    void setServerSocket(int socket);
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

    void sendImage(const cv::Mat &image)
    {
        std::vector<uint8_t> buffer;
        // cv::imencode(".jpg", image, buffer);
        std::cout << "Sending Image" << std::endl;
    }
};

/**
 *
 */
Client::~Client()
{
    close(this->clientSocket);
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
void Client::setServerSocket(int socket)
{
    // Check Client is in IDLE state
    if (this->state != IDLE_STAGE)
    {
        throw std::exception();
    }

    // Check socket is valid
    if (socket > 0 && socket < 65536)
    {
        this->serverSocket = socket;
    }
    else
    {
        throw std::exception();
    }
    return;
}

/**
 *
 */
void Client::connectToServer()
{
    // Check Client is in IDLE stage
    if (this->state != IDLE_STAGE)
    {
        throw ClientException{std::format("ERROR: Client in wrong state\nExpected: IDLE_STAGE(0)\nActual: {}", this->state), 1};
    }

    // Proceed with Connection
    this->serverAddr = {
        AF_INET,
        htons(this->serverSocket),
        INADDR_ANY};

    // Server running on Localhost
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(this->clientSocket, (struct sockaddr *)&this->serverAddr, sizeof(this->serverAddr)) < 0)
    {
        throw ClientException("ERROR: Client could not connect on provided port", 2);
    }

    // Update state to REQ(UEST) STAGE, indactes client is ready to send request to server
    this->state = REQ_STAGE;
}

/**
 * @brief Send a request containing desired frames to server
 */
void Client::sendRequestSrv()
{
    char buffer[1024] = {'\0'};
    cv::Mat img;

    nlohmann::json j = "{ \"Success\": 0 }"_json;
    nlohmann::json request;
    std::string requestHash;

    // Check Client is in REQ_STAGE
    if (this->state != REQ_STAGE)
    {
        throw ClientException(std::format("ERROR: Client in wrong state\nExpected: REQ_STAGE(1)\nActual: {}", this->state));
    }

    // Generate Request
    request["id"] = 0;
    request["frames"] = {SatColor::RED, SatColor::GREEN, SatColor::BLUE};

    requestHash = md5(request.dump().c_str());
    request["hash"] = requestHash;

    // // DEBUG
    // unsigned char buFF[1024];
    // std::string hash = md5("hello");
    // std::cout << "MD5 hash: " << hash << std::endl;




    // Place Request in Buffer, check size constraints 
    size_t requestSize = request.dump().size();
    if ( requestSize > 1023 ) {
        throw ClientException( std::format("BAD_REQUEST::ERROR: Client Request Exceeded 1024 byte limit, Actual {}", requestSize), 1);
    } else {
        strcpy( buffer, request.dump().c_str() );
    }
    send(clientSocket, buffer, sizeof(buffer), 0);



    // Clear buffer
    for(int i = 0; i < sizeof(buffer); i++) {
        buffer[i] = '\0';
    }



    // Request Sent, wait for FIN response indicating last packet/frame
    send(clientSocket, reinterpret_cast<const char *>("Client HELO"), 20, 0);
    recv(clientSocket, buffer, sizeof(buffer), 0);

    // Try to receive Image frame from server
    try
    {
        std::string res = buffer;
        std::vector<uchar> mem_buffer;
        size_t buff_size;

        // Parse JSON header from server
        j = nlohmann::json::parse(res.begin(), res.end());

        // Receive buffer size from server, size buffer accordingly
        recv(clientSocket, &buff_size, sizeof(size_t), 0);
        mem_buffer.resize(buff_size);

        // Receive data into buffer on client, decode into cv::Mat object
        recv(clientSocket, mem_buffer.data(), buff_size, 0);
        img = cv::imdecode(mem_buffer, cv::IMREAD_COLOR);

        // Check that IMG is NOT empty, if it is throw an error
        std::cout << "Mem Buffer Size: " << mem_buffer.size() << std::endl;
        if (img.empty())
        {
            std::cerr << "Error: Image element is empty" << std::endl;
        }

        // Display Image
        cv::imshow("Image demo", img);
        cv::waitKey(0);
    }
    catch (nlohmann::json::parse_error &err)
    {
        std::cerr << "JSON::ERROR: " << buffer << std::endl;
        std::cerr << "JSON::ERROR: " << err.what() << std::endl;
        return;
    }
    catch (std::exception &exc)
    {
        std::cerr << "STND::ERROR: " << exc.what() << std::endl;
    }
}

int main(int argc, char **argv)
{
    Client hello_client;
    int port(39554);

    try
    {
        if (argc < 2)
        {
            hello_client.setServerSocket(port);
            hello_client.connectToServer();
            hello_client.sendRequestSrv();
        }
        else
        {
            try
            {
                port = std::stoi(argv[1]);
            }
            catch (std::exception &exc)
            {
                std::cerr << "Invalid Port Specified: " << std::endl;
                return RETURN_USR_ERR;
            }

            hello_client.setServerSocket(port);
            hello_client.connectToServer();
            hello_client.sendRequestSrv();
        }
    }
    catch (std::exception &exc)
    {
        std::cerr << "An Unexpected Error Occurred: " << exc.what() << std::endl;
    }

    return RETURN_OK;
}