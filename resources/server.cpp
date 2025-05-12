
#define NUMBER_FRAMES 3

#include "server.h"


void Server::setListeningPort( int port ) {
    if( ! ( 0 < port && port < 65535 ) ) {
        throw ServerException("error: port number invalid", 0);
    }
    this->serverPort = port;
    return;
}

void Server::setListeningAddress( const std::string& listeningAddress ) {
    if( validIPv4Listening(listeningAddress) ) {
        this->listenAddr = listeningAddress;
    } else {
        throw ServerException(std::format("ServerException: Invalid IPv4 listening address passed ({})", listeningAddress));
    }
    return;
}

// Data Accessors
std::string Server::getListeningAddress() const {
    return this->listenAddr;
}

int Server::getListeningPort() const {
    return this->server_sin.sin_port;
}

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
    this->listenAddr = mAddr;
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

    // Setup Server Port
    this->server_sin = {
        AF_INET,
        htons(this->serverPort),
        INADDR_ANY};

    // Listen on ALL IP addresses
    server_sin.sin_addr.s_addr = inet_addr(this->listenAddr.c_str());

    bind(this->serverSocket, (struct sockaddr *)&server_sin, sizeof(server_sin));
    this->state = RDY_STAGE;
    return;
}

/**
 *
 */
void Server::serverLoop()
{
    // If Server Not Setup, Don't run
    if (this->state != RDY_STAGE)
    {
        throw std::runtime_error("Server is not in the ready state to start the loop.");
    }

    // Main Server Loop
    std::cout << std::format("Server Listening on: {}\n", ntohs(this->server_sin.sin_port) );
    std::cout << std::format("Server Socket: {}\n", this->serverSocket);
    std::cout << "-----------------------------\n";
    
    try {
        // Run Server Indefinitely
        for (;;)
        {
            int clientSocket;
            char buffer[1024] = {0};
            
            // Listen for Incoming Connections
            listen(serverSocket, 5);
            clientSocket = accept(serverSocket, nullptr, nullptr);
            
            // Create and Detach thread
            std::thread thr(&Server::client_handle, this, clientSocket);
            thr.detach(); // Detach the thread to allow it to run independently
        }
    }
    catch(ServerException& exc) {
        std::cerr << "Server Exception: " << exc.what() << std::endl;
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

        // Camera failed to open, send default.png
        img = cv::imread("../assets/default.png");
        std::cerr << "Could not open Camera Feed" << std::endl;
        if( img.empty() ) throw ServerException("Could not open default image", 0);
        return img;
        // throw ServerException({"Camera::ERROR: Could not open media", 1});
    } else {

        // Read Image and Filter
        cap.read(img);
        cap.release();
        cv::bilateralFilter(img, filtered, 50, 25, 25);

        // Camera opened, but no image could be read
        if( img.empty() ) {
            img = cv::imread("../assets/default.png");
            std::cerr << "Could not read frame from camera" << std::endl;
            if( img.empty() ) throw ServerException("Could not open default image", 0);
            return img;

        // Image Read, but filtered could not be produced
        } else if( filtered.empty() ){
            img = cv::imread("../assets/default.png");
            std::cerr << "Could not produce filtered frame" << std::endl;
            if( img.empty() ) throw ServerException("Could not open default image", 0);
            return img;
        }
    }
    return filtered;
}

/**
 * @brief Handle Client connections to the server.
 * @details Process the client's request, generate the desired output,
 *          and send over sockets back to the user the requested data.
 * @return Nothing
 */
void Server::client_handle(int client_socket) {
    std::string buffer;
    nlohmann::json request;
    std::vector<uchar> imgBuff;
    size_t buffer_size(0);
    std::vector<Filter> colorFilters;
    std::vector<std::pair<cv::Mat, std::string>> frames;

    // Receive Request Size First, then receive request
    recv(client_socket, &buffer_size, sizeof(size_t), 0);
    buffer.resize(buffer_size);
    recv(client_socket, buffer.data(), buffer_size, 0);
    
    // Parse Request, check integrity, and check for correct state
    request = nlohmann::json::parse( buffer.begin(), buffer.end() );
    if( ! checkHashJSON(request) ) {
        throw ServerException("JSON hash incorrect");
    }
    if( request["state"] != "request" ) {
        throw ServerException("Client not in request state");
    }
    colorFilters = buildFilterArray( request );


    // Retrieve Camera Image and Process into Frames
    cv::Mat img = getCameraFrame();
    imageProc(img, colorFilters, frames);
    
    // Send Client Number of Frames to Accept
    // nlohmann::json j;
    // j["num_frames"] = frames.size();
    // send(client_socket, j.dump().data(), j.dump().size(), 0);
    int numberFrames(frames.size());

    send(client_socket, &numberFrames, sizeof(int), 0);
    int i = 0;
    std::cout << "Frames Size: " << frames.size() << std::endl;
    std::cout << "Request Frames Size: " << request["frames"].size() << std::endl;

    for( auto pair : frames ) {
        size_t img_buff_size(0);

        // Encode Frame
        cv::imencode( ".png", pair.first, imgBuff );
        img_buff_size = imgBuff.size();

        // Send Integer Indicating Saturation Color
        send(client_socket, &i, sizeof(int), 0);

        // Send Frame to Client
        send(client_socket, &img_buff_size, sizeof(size_t), 0);
        send(client_socket, imgBuff.data(), img_buff_size, 0);

        std::cout << "Filter: " << request["frames"].at(i) << std::endl;
        std::cout << "MD5 Hash: " << pair.second << std::endl;

        // DEBUG
        buffer.resize(10);
        recv(client_socket, buffer.data(), sizeof(10), 0);
        // std::cout << "Received: " << buffer << std::endl;
        std::cout << "Count: " << i << " of 3" << std::endl; 
        i++;
    }
    std::cout << "--------------------" << std::endl;

    ///////////
    // std::vector<std::pair<cv::Mat, std::string>> frames;
    // imageProc(  )
    close(client_socket);
    return;
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
    // if (this->state != REQ_STAGE)
    // {
    //     throw ServerException(std::format("Image::Proc: In state ({}) when expected was REQ_STAGE(3)", this->state), 0);
    // }
    // else if (input.empty())
    // {
    //     throw ServerException("ImageProc::ERROR: Passed input was empty", 0);
    // }
    // else if (!filters.size())
    // {
    //     throw ServerException("ImageProc::ERROR: Filters array is empty", 0);
    // }

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
        throw ServerException("FilterArr::ERROR: Passed request is missing frames attribute", 0);
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