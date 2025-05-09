#include <iostream>
#include "server.h"

// int main() {
//     std::cout << "Hello World!" << std::endl;
//     return 0;
// }

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