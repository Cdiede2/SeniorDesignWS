#include <iostream>
#include "server.h"

// int main() {
//     std::cout << "Hello World!" << std::endl;
//     return 0;
// }

int main(int argc, char **argv)
{
    Server serverObject;    // Server Object
    std::string ipAddress;  // Listening address
    int port(0);            // Listening port

    try {
        switch( argc ) {
            case 1:
                std::cerr << "usage: CamServer <ip-address> [port]" << std::endl;
                return RETURN_USR_ERR;
                break;
            
            // User Entered IP Address Only
            case 2:
                ipAddress = argv[1];
                serverObject.setListeningAddress(ipAddress);
                serverObject.setListeningPort(39554);
                break;
            
            // User enters both IP and PORT
            case 3:
                ipAddress = argv[1];
                port = std::stoi(argv[2]);
                serverObject.setListeningAddress(ipAddress);
                serverObject.setListeningPort(port);
                break;
                
            // Default Case
            default:
                std::cerr << "usage: CamServer <ip-address> [port]" << std::endl;
                return RETURN_USR_ERR;
                break;

        };
    } catch( ServerException& exc ) {
        std::cerr << "error: " << exc.what() << std::endl;
        return RETURN_USR_ERR;
    } catch( std::exception& exc ) {
        std::cerr << "error: " << exc.what() << std::endl;
        return RETURN_USR_ERR;
    }

    try
    {
        std::cout << "Server Listening Address: " << serverObject.getListeningAddress() << std::endl;
        std::cout << "Server Listening Port: " << serverObject.getListeningPort() << std::endl;
        // return RETURN_OK;

    
        // Run Server Code
        serverObject.setupServer();
        serverObject.serverLoop();
    }
    catch (ServerException &exc)
    {
        std::cerr << "ServerException:\n" << exc.what() << std::endl;
    }
    catch ( cv::Exception &exc) {
        std::cerr << "OpenCV::Exception:\n" << exc.what() << std::endl;
    }
    catch (std::exception &exc)
    {
        std::cerr << "Std::Exception:\n" << exc.what() << std::endl;
    }
    return RETURN_OK;
}