#ifndef LOGINSESSION_H
#define LOGINSESSION_H

#include "Session.h"
#include "Lobby.h"

using boost::asio::ip::tcp;

#define MAX_BUFFER 1024

class LoginSession : public Session, public std::enable_shared_from_this<LoginSession>
{
    //Class for managing a single connection with a client
    private:
        tcp::socket socket;
        char data[MAX_BUFFER];
        std::string name; //username associated with session
        Lobby& lobby; //shared lobby object

        std::string GetName();

        //read data from current session's socket
        void DoRead();

        //write data to server
        void DoWrite();

    public:
        LoginSession(tcp::socket socket_, Lobby& lobby_)
            : socket(std::move(socket_)), lobby(lobby_) {}

        //start reading from connection
        void Start();
};

#endif
