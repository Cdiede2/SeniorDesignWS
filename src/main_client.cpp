#include <iostream>
#include "client.h"


// int main() {
//     std::cout << "hello World" << std::endl;
//     return 0;
// }


int main(int argc, char **argv)
{
    Client clientObject;

    int port(39554);

    std::string ipAddress;
    try {

        switch( argc ) {
            // User Passed NO Arguments to CLI
            case 1:
                std::cerr << "usage: CamClient <ip-address> [port]" << std::endl;
                return RETURN_USR_ERR;

            // User Passed One Argument to CLI
            case 2:
                ipAddress = argv[1];
                clientObject.setServerAddress( ipAddress );
                clientObject.setServerPort( port );
                break;

            // User passed Two Arguments in CLI
            case 3:
                ipAddress = argv[1];
                port = std::stoi(argv[2]);
                clientObject.setServerAddress( ipAddress );
                clientObject.setServerPort( port );
                break;

            // User likely passed more than 2 Arguments
            default:
                std::cerr << "usage: CamClient <ip-address> [port]" << std::endl;
                return RETURN_USR_ERR;
        }

    // Client Exception Thrown
    } catch ( ClientException& exc ) {
        std::cerr << "Error: " << exc.what() << "\n";
        std::cerr << "usage: CamClient <ip-address> [port]" << std::endl;
    }  

    // Standard Exception thrown
    catch ( std::exception& exc ) {
        std::cerr << "Error: " << exc.what() << "\n";
        std::cerr << "usage: CamClient <ip-address> [port]" << std::endl;
    }

    std::cout << std::format("Server Address: {}\n", clientObject.getServerAddress() );
    std::cout << std::format("Server Port: {}\n", clientObject.getServerPort());

    try
    {
        clientObject.connectToServer();
        clientObject.sendRequestSrv();
    }
    catch (std::exception &exc)
    {
        std::cerr << "An Unexpected Error Occurred: " << exc.what() << std::endl;
    }
    catch( ClientException& exc ) {
        std::cout << "Client Exception: " << exc.what() << std::endl;
    }
    return RETURN_OK;
}