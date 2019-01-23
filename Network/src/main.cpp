#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <string>
#include <numeric>
#include <boost/asio.hpp>
#include <boost/algorithm/string/join.hpp>
#include "Server.h"

#define DEFAULT_PORT 55107 //same default port of game client

int main(int argc, char* argv[]) {
    try {
        int Port;

        if (argc > 2) { //more arguments than expected
            std::cerr << "Usage: async_tcp_echo_server [port]\n";
            return 1;
        }
        else if (argc == 2) { //port provided
            Port = std::atoi(argv[1]);
        }
        else { //no port specified
            std::cout << "No port specified, using 55107 as default" << std::endl;
            Port = DEFAULT_PORT;
        }

        boost::asio::io_service io_service;

        //create serever object
        Server s(io_service, Port);
        std::cout << "Server started!" << std::endl;

        //run io_service loop
        io_service.run();
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
