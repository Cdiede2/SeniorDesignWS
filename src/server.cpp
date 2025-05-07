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
// #include <openssl/md5.h>

// Networking
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

// #include "shannon-fano.h"
#include "camera.h"       // Split and Strip functions
#include "shannon-fano.h" // Shannon-Fano Encoding

#define NUMBER_FRAMES 3

enum numbers
{
    RETURN_OK,
    RETURN_NETWORK_ERR,
    RETURN_USR_ERR
};

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
    void setServerPort(const char *mAddr, int port); // Setup the server Address and Port
    void setupServer();                              // Create listening socket
    cv::Mat getCameraFrame();                        // Access media and retrieve image

    // Listening Loop
    void serverLoop(); // Main server loop

private:
    uint16_t serverPort;
    uint8_t state;
    std::string mcastAddr;
    int serverSocket;
    struct sockaddr_in serverAddress;

    // Private Client Handle
    void client_handle(int client_socket); // Client thread
    bool imageProc(const cv::Mat &input, const std::vector<Filter> &filters, std::vector<std::pair<cv::Mat, std::string>> &ret_val);
    std::vector<Filter> buildFilterArray( nlohmann::json& request );
};

/**
 * @brief Set the server listening port and address
 * @param mAddr const char*: multicast listening address
 * @param port int: listening port
 */
void Server::setServerPort(const char *mAddr, int port)
{
    // Check that port is valid
    if (!(port > 0 && port < 65536))
    {
        throw ServerException(std::format("SETUP::ERROR: Invalid port range specified {}", port), 0);

        // throw ServerException("SETUP::ERROR: Invalid port range specified", 0);
    }
    this->mcastAddr = mAddr;
    this->serverPort = static_cast<uint16_t>(port);
    return;
}

/**
 *
 */
void Server::setupServer()
{
    // Validate stage
    if (this->state != IDLE_STAGE)
    {
        throw std::runtime_error("Server Still in Idle");
    }
    else if (!(this->serverPort > 0 && this->serverPort < 65536))
    {
        throw ServerException({"SETUP::ERROR: Invalid port range specified", 1});
    }

    // Seup Server Port
    this->serverAddress = {
        AF_INET,
        htons(this->serverPort),
        INADDR_ANY};

    // Listen on ALL IP addresses
    serverAddress.sin_addr.s_addr = inet_addr(this->mcastAddr.c_str());

    bind(this->serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    this->state = RDY_STAGE;
    return;
}

/**
 *
 */
void Server::serverLoop()
{
    if (this->state != RDY_STAGE)
    {
        throw std::runtime_error("Server is not in the ready state to start the loop.");
    }

    // Main Server Loop
    std::cout << std::format("Server Listening on: {}", this->serverPort) << std::endl;
    std::cout << this->serverSocket << std::endl;

    cv::Mat image = cv::imread("../image.jpg", cv::IMREAD_COLOR);
    for (;;)
    {
        int clientSocket;
        char buffer[1024] = {0};

        // Listen for Incoming Connections
        listen(serverSocket, 5);
        clientSocket = accept(serverSocket, nullptr, nullptr);

        // Read an image from the camera or file
        if (image.empty())
        {
            std::cerr << "Error: Could not read the image." << std::endl;
            close(clientSocket);
            continue;
        }

        // DEBUG: Display the image in a window
        // cv::imshow("Server Image Display", image);
        // cv::waitKey(1); // Allow the window to refresh

        std::thread thr(&Server::client_handle, this, clientSocket);
        thr.detach(); // Detach the thread to allow it to run independently
    }
    return;
}

/**
 * @brief Retrieve a single camera frame from connected Camera at device 0
 */
cv::Mat Server::getCameraFrame()
{
    cv::Mat img;
    cv::Mat filtered;

    // Attempt to Open Camera, on failure throw error
    auto cap = cv::VideoCapture(0);
    if (!cap.isOpened())
    {
        throw ServerException({"Camera::ERROR: Could not open media", 1});
    }

    // Read Image and Filter
    cap.read(img);
    cap.release();
    cv::bilateralFilter(img, filtered, 50, 25, 25);

    if (img.empty())
    {
        throw ServerException("Camera::ERROR: Camera was read, but no media was created", 0);
    }
    else if (filtered.empty())
    {
        throw ServerException("Camera::ERROR: Filtered Image is empty.", 0);
    }
    std::cout << "Camera::SUCCESS: Successfully, pulled image from camera" << std::endl;
    return filtered;
}

/**
 * @brief Handle Client connections to the server.
 * @details Process the client's request, generate the desired output,
 *          and send over sockets back to the user the requested data.
 * @return Nothing
 */
void Server::client_handle(int client_socket)
{
    char buffer[1024] = {0};
    cv::Mat img = cv::imread("../image.jpg", cv::IMREAD_COLOR);
    nlohmann::json recvRequest;
    std::string hash;
    std::string str_buffer;
    size_t sizeV(0);
    std::vector<std::pair<cv::Mat, std::string>> resultant_imgs;
    std::vector<Filter> filters;

    try {
        
    }
    // Receive Client Request, Parse JSON to retrieve desired color filters
    recv(client_socket, buffer, sizeof(buffer), 0);
    str_buffer = buffer;
    recvRequest = nlohmann::json::parse(str_buffer.begin(), str_buffer.end());
    hash = recvRequest["hash"];
    recvRequest.erase("hash");

    // Validate Request HASH. On mismatch, network error may have occurred.
    if (hash != md5(recvRequest.dump().c_str()))
    {
        std::cout << "Hash Mismatch" << std::endl;
    }
    else
    {
        std::cout << "Checksums match!" << std::endl;
    }

    // Retrieve Filters from JSON
    filters = buildFilterArray( recvRequest );

    std::cout << "Filters: \n" << filters << std::endl;
    std::cout << "String Buffer: " << recvRequest << std::endl;
    std::cout << recvRequest["frames"][0] << recvRequest["frames"][1] << recvRequest["frames"][2] << std::endl;

    this->state = REQ_STAGE;
    try {
        bool success = imageProc( img, filters,  resultant_imgs );

    } catch( ServerException& exc ) {
        std::cerr << exc.what << std::endl;
        return;
    }

    int frame_id = 1;
    for( auto pair : resultant_imgs ) {
        std::cout << "MD5 Hash: " << pair.second << std::endl;
        cv::imshow(std::format("Frame ID: {}", frame_id++).c_str(),pair.first);
        cv::waitKey(0);
    }
    // if( !success ) {
    //     throw ServerException("ImageProc::ERROR: Failed to process frames", 0);
    // }

    // Does not work on unconfigured WSL
    // Check: https://askubuntu.com/questions/1405903/capturing-webcam-video-with-opencv-in-wsl2
    // getCameraFrame();

    // Receive Client Hello
    recv(client_socket, buffer, sizeof(buffer), 0);

    // Header Includes Number of Frames and JSON frame to Saturation Color
    nlohmann::json header{
        {"im_width", img.cols},
        {"im_height", img.rows},
        {"im_depth", 3},
        {"sat_color", SatColor::RED}};

    send(client_socket, header.dump().c_str(), header.dump().size(), 0);
    std::cout << "Serialized JSON Send" << std::endl;

    // Only execute below code if IMG is NOT empty
    if (!img.empty())
    {
        // Create Buffer and Size variables
        std::vector<uchar> buff;
        size_t size;

        // Encode the Image in some format
        cv::imencode(".png", img, buff);
        size = buff.size();

        std::cout << size << std::endl;

        // FIRST: Send client the compressed image buffer size
        // Second: Send the entire compressed image data over socket
        send(client_socket, &size, sizeof(size_t), 0);
        send(client_socket, buff.data(), buff.size(), 0);
    }

    close(client_socket);
}

/**
 * @brief Split Image into frames and compute MD5 hash for each frame, individually.
 * @param input Image to be processed by function call
 * @param filters Color vectors to be applied to input as filters
 * @param ret_val The final results of processing will be written here
 * @return bool indicating the success or failure of the operation
 */
bool Server::imageProc(const cv::Mat &input, const std::vector<Filter> &filters, std::vector<std::pair<cv::Mat, std::string>> &ret_val)
{

    // Check input and filters are NOT EMPTY
    if (this->state != REQ_STAGE)
    {
        throw ServerException(std::format("Image::Proc: In state ({}) when expected was REQ_STAGE(3)", this->state), 0);
    }
    else if (input.empty())
    {
        throw ServerException("ImageProc::ERROR: Passed input was empty", 0);
    }
    else if (!filters.size())
    {
        throw ServerException("ImageProc::ERROR: Filters array is empty", 0);
    }

    // NOTE: For now, only color vectors RGB are supported
    assert(filters.at(0) == Filter(255, 0, 0));
    assert(filters.at(1) == Filter(0, 255, 0));
    assert(filters.at(2) == Filter(0, 0, 255));

    for (auto filt : filters)
    {
        // Create a copy of the input image
        cv::Mat filteredImage;
        input.copyTo(filteredImage);

        // Apply the filter by zeroing out other channels
        std::vector<cv::Mat> channels(3);
        cv::split(filteredImage, channels);

        if (filt == Filter(255, 0, 0)) // Red filter
        {
            channels[1] = cv::Mat::zeros(channels[1].size(), channels[1].type()); // Zero out Green
            channels[2] = cv::Mat::zeros(channels[2].size(), channels[2].type()); // Zero out Blue
        }
        else if (filt == Filter(0, 255, 0)) // Green filter
        {
            channels[0] = cv::Mat::zeros(channels[0].size(), channels[0].type()); // Zero out Red
            channels[2] = cv::Mat::zeros(channels[2].size(), channels[2].type()); // Zero out Blue
        }
        else if (filt == Filter(0, 0, 255)) // Blue filter
        {
            channels[0] = cv::Mat::zeros(channels[0].size(), channels[0].type()); // Zero out Red
            channels[1] = cv::Mat::zeros(channels[1].size(), channels[1].type()); // Zero out Green
        }

        // Merge the channels back together
        cv::merge(channels, filteredImage);

        // Compute MD5 hash for the filtered image
        std::vector<uchar> buffer;
        cv::imencode(".png", filteredImage, buffer);
        std::string hash = md5({reinterpret_cast<const char *>(buffer.data())});

        // Append the result to the return vector
        ret_val.emplace_back(filteredImage, hash);
    }

    // For each filter, apply to IMG and append to ret_val
    // for( auto filt : filters ) {

    // }
    return true;
}

/**
 * @brief Generates Filters from Request
 * @param request the JSON object which contains the filters list
 */
std::vector<Filter> Server::buildFilterArray( nlohmann::json& request ) {
    if( ! request.size() ) {
        throw ServerException("FilterArr::ERROR: Passed request is of size 0", 0);
    }
    
    nlohmann::json::iterator it = request.end();
    if( request.find("frames") == it ) {
        throw ServerException("FilterArr::ERRROR: Passed request is missing frames attribute", 0);
    }
    
    std::vector<Filter> filters;
    std::vector<int> frames = request["frames"];
    if ( frames.at(0) == SatColor::RED)
    {
        filters.push_back(Filter(255, 0, 0));
    }
    else
    {
        std::cout << "No Red" << std::endl;
    }

    if (frames.at(1) == SatColor::GREEN)
    {
        filters.push_back(Filter(0, 255, 0));
    }
    else
    {
        std::cout << "No grn" << std::endl;
    }

    if (frames.at(2) == SatColor::BLUE)
    {
        filters.push_back(Filter(0, 0, 255));
    }
    else
    {
        std::cout << "No Blu" << std::endl;
    }
    return filters;
}

int main(int argc, char **argv)
{
    Server helloServer;
    try
    {
        
        // Read CMD line Arguments
        if (argc < 2)
        {
            helloServer.setServerPort("127.0.0.1", 39554);
        }
        else if (argc < 3)
        {
            std::cerr << "This function is broken at this moment. Please pass command with 0 parameters for now." << std::endl;
            std::cout << "\e[33m Value: " << std::stoi(argv[1]) << "\e[0m" << std::endl;
            helloServer.setServerPort("127.0.0.1", std::stoi(argv[1]));
        }
        else
        {
            std::cerr << "This function is broken at this moment. Please pass command with 0 parameters for now." << std::endl;
            return RETURN_USR_ERR;

            std::cout << "\e[33m Value: " << argv[1] << "\e[0m" << std::endl;
            std::cout << "\e[33m Value: " << std::stoi(argv[2]) << "\e[0m" << std::endl;
            helloServer.setServerPort(argv[2], std::stoi(argv[1]));
        }

        // Run Server Code
        helloServer.setupServer();
        helloServer.serverLoop();
    }
    catch (ServerException &exc)
    {
        std::cerr << exc.what << std::endl;
    }
    catch (std::exception &exc)
    {
        std::cerr << exc.what() << std::endl;
    }
    return RETURN_OK;
}