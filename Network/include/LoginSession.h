#ifndef LOGINSESSION_H
#define LOGINSESSION_H

#include "Session.h"
#include "Lobby.h"
#include "User.h"

using boost::asio::ip::tcp;

class LoginSession : public Session, public std::enable_shared_from_this<LoginSession>
{
    //Class for managing a single connection with a client
    private:
        /*tcp::socket socket;
        char data[MAX_BUFFER];
        std::string name; //username associated with session
        Lobby& lobby; //shared lobby object*/

        //std::string GetName();

        //read data from current session's socket
        void DoRead(User_ptr UserPtr);

        //write data to server
        void DoWrite(User_ptr UserPtr);

        //start reading from connection
        void Start(User_ptr UserPtr);

    public:
        LoginSession() {}
};

#endif
