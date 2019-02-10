#ifndef SERVER_H
#define SERVER_H

#include "Lobby.h"
#include "User.h"

// Server class to handle connection (asynchronous for now)
class Server
{
    private:
        boost::asio::io_service& io_service;
        tcp::acceptor acceptor;
        tcp::socket socket;
        Lobby lobby;


        void DoAccept();
    public:
        Server(boost::asio::io_service& io_service_, short port);
};

#endif
