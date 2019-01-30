#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

#define BUFFER_SIZE 1024

class CApplicationData;

class Client
{
    public:
    // members
        boost::asio::io_service io_service;
        tcp::socket socket;
        tcp::resolver resolver;

    // member functions
        Client();
        bool Connect(std::shared_ptr<CApplicationData> context);
        bool SendLoginInfo(std::shared_ptr<CApplicationData> context);
        void SendGameInfo(std::shared_ptr<CApplicationData> context);
        void SendMessage(std::string message);
        void CloseConnection();
};
#endif
