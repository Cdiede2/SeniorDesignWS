#include "client.h"

/**
 *
 */
Client::~Client()
{
    close(this->clientSocket);
}

/**
 * @brief Establishes a connection to the server.
 *
 * This method attempts to connect the client to the server using the specified
 * server address and port. It validates the client's current state to ensure
 * it is in the IDLE_STAGE before proceeding with the connection. If the connection
 * is successful, the client's state is updated to REQ_STAGE, indicating readiness
 * to send requests to the server.
 *
 * @throws ClientException If the client is not in the IDLE_STAGE.
 * @throws ClientException If the connection to the server fails.
 */
void Client::connectToServer()
{
    // Check Client is in IDLE stage
    if (this->state != IDLE_STAGE)
    {
        throw ClientException{std::format("ERROR: Client in wrong state\nExpected: IDLE_STAGE(0)\nActual: {}", this->state), 1};
    }

    // Proceed with Connection
    this->server = {
        AF_INET,
        htons(this->serverPort),
        INADDR_ANY};

    // Server running on Localhost
    // server.sin_addr.s_addr = inet_addr("127.0.0.1");

    server.sin_addr.s_addr = inet_addr(this->serverAddr.c_str());

    if (connect(this->clientSocket, (struct sockaddr *)&this->server, sizeof(this->server)) < 0)
    {
        throw ClientException(std::format("ERROR: Client could not connect to {}:{}", (server.sin_addr.s_addr), (this->serverPort)), 2);
    }

    // Update state to REQ(UEST) STAGE, indicates client is ready to send request to server
    this->state = REQ_STAGE;
}

void Client::sendRequestSrv() {
    nlohmann::json request;
    size_t requestSize(0);
    
    // Form Request
    request["state"] = "request";
    request["id"] = 0;
    request["frames"] = {SatColor::RED, SatColor::GREEN, SatColor::BLUE};
    request["hash"] = md5( request.dump().c_str() );
    requestSize = request.dump().size();
    
    // Send Buffer Size and Request
    send(clientSocket, &requestSize, sizeof(size_t), 0);
    send(clientSocket, request.dump().data(), requestSize, 0 );

    std::vector<cv::Mat> imgs;
    imgs = recvFrames();
   
    // recvFrames();
    // cv::imshow("Image", img);
    // cv::waitKey(0);
    return;
}

std::vector<cv::Mat> Client::recvFrames() {
    int numFrames(0);

    std::vector<uchar> buffer;
    std::vector<cv::Mat> imgs;


    // Receive Number of Frames to Accept
    recv(clientSocket, &numFrames, sizeof(int), 0);
    std::cout << "Number of Frames: " << numFrames << std::endl;
    for( int i = 0; i < numFrames; i++ ) {
        cv::Mat img;
        size_t buffer_size(0);
        size_t bytes_expected(0);
        size_t bytes_received(0);
        int saturationColor(-1);

        // Receive Header for Frame
        recv(this->clientSocket, &saturationColor, sizeof(int), 0);

        // Receive Buffer Size and Reshape
        recv(this->clientSocket, &buffer_size, sizeof(size_t), 0);
        buffer.resize(buffer_size);
        bytes_expected = buffer_size;

        while( bytes_received < bytes_expected ) {
            bytes_received += recv(this->clientSocket, buffer.data() + bytes_received, buffer_size - bytes_received, 0);
        }

        // DEBUG: Image Received in Full
        send(clientSocket, reinterpret_cast<const char*>("OK"), sizeof(10), 0);

        // Decode Image
        imgs.push_back( cv::imdecode(buffer, cv::IMREAD_COLOR) );
    }

    for( auto val : imgs ) {
        cv::imshow("Frame", val);
        cv::waitKey(0);
    }
    return imgs;
}


/**
 * @brief Send a request containing desired frames to server
 */
/*void Client::sendRequestSrv()
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

    // Place Request in Buffer, check size constraints
    size_t requestSize = request.dump().size();
    if (requestSize > 1023)
    {
        throw ClientException(std::format("BAD_REQUEST::ERROR: Client Request Exceeded 1024 byte limit, Actual {}", requestSize), 1);
    }
    else
    {
        strcpy(buffer, request.dump().c_str());
    }
    send(clientSocket, buffer, sizeof(buffer), 0);

    // Clear buffer
    for (int i = 0; i < sizeof(buffer); i++)
    {
        buffer[i] = '\0';
    }

    //// Client Receives JSON header indicating number of frames to be sent
    // Request Sent, wait for FIN response indicating last packet/frame
    // send(clientSocket, reinterpret_cast<const char *>("Client HELO"), 20, 0);
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

        //// Client Receives SINGLE Image here
        // Receive data into buffer on client, decode into cv::Mat object
        recv(clientSocket, mem_buffer.data(), buff_size, 0);
        
        std::cout << "Size: " << mem_buffer.size() << std::endl;        
        img = cv::imdecode(mem_buffer, cv::IMREAD_UNCHANGED);
        
        // DEBUG: Remove
        std::cout << "Image: " << std::endl;
        std::cout << "Image Size: " << img.size() << std::endl;
        std::cout << "Image Rows: " << img.rows << std::endl;
        std::cout << "Image Cols: " << img.cols << std::endl;


        cv::imshow("Image Debug", img);
        cv::waitKey(0);

        // img = cv::imread("../assets/default.png");

        // Check that IMG is NOT empty, if it is throw an error
        std::cout << "Mem Buffer Size: " << mem_buffer.size() << std::endl;
        if (img.empty())
        {
            std::cerr << "Error: Image element is empty" << std::endl;
            throw ClientException("Error: Image element is empty");
        }

        // Display Image
        std::cout << "Image: " << img << std::endl;

        // cv::imshow("Image demo", img);
        // cv::imwrite("output.jpg", img);
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
}*/


/////////////////////////////////////
// Accessors
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
 * @throws std::exception If the provided socket value is invalid (not in the range [1024, 65535]).
 */
void Client::setServerPort(int socket)
{
    // Check Client is in IDLE state
    if (this->state != IDLE_STAGE)
    {
        throw std::exception();
    }

    // Check socket is valid
    if (socket > 1023 && socket < 65536)
    {
        this->serverPort = socket;
    }
    else
    {
        throw std::exception();
    }
    return;
}

/**
 * @brief Sets the server address for the client.
 *
 * This method assigns an IPv4 address to the client, ensuring that the provided
 * address is valid. It performs several checks to validate the format and content
 * of the IP address, including:
 * - Ensuring the address contains exactly three periods ('.').
 * - Verifying the length of the address does not exceed the maximum possible length
 *   for an IPv4 address (15 characters).
 * - Ensuring all characters in the address are either digits or periods.
 * - Splitting the address into octets and confirming each octet is within the valid
 *   range of 0 to 255.
 *
 * If any of these checks fail, a `ClientException` is thrown with an appropriate
 * error message.
 *
 * @param ipAddress A string representing the IPv4 address to be set for the server.
 *
 * @throws ClientException If the IP address contains an invalid format, exceeds
 *         the maximum length, contains non-numeric characters, or has octets
 *         outside the valid range.
 */
void Client::setServerAddress(const std::string &ipAddress)
{
    // Check that only valid IP Addresses are assigned
    if( ! validIPv4(ipAddress) ) {
        throw ClientException("ClientError: Server IP is invalid");
    }
    this->serverAddr = ipAddress;
    return;
}

/**
 * @brief Retrieves the server port associated with the client.
 *
 * This method returns the port number that the client is configured to use
 * for connecting to the server. The port number is stored as an integer
 * in the client object and represents the server's listening port.
 *
 * @return An integer representing the server's port number.
 */
int Client::getServerPort() const
{
    return this->serverPort;
}

/**
 * @brief Retrieves the server address associated with the client.
 *
 * This method returns the IP address of the server to which the client
 * is configured to connect. The address is stored as a string in the
 * client object and represents the server's IPv4 address.
 *
 * @return A string containing the server's IP address.
 */
std::string Client::getServerAddress() const
{
    return this->serverAddr;
}