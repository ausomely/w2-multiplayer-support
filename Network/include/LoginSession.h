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
        struct SPrivateSessionType{};
    protected:
        static std::shared_ptr< Session > DLoginSessionPointer;
    public:
        explicit LoginSession(const SPrivateSessionType &key) {}

        static std::shared_ptr< Session > Instance();

        //read data from current session's socket
        void DoRead(User_ptr UserPtr);

        //write data to server
        void DoWrite(User_ptr UserPtr);

        //start reading from connection
        void Start(User_ptr UserPtr);

        //restart and read data from client again if fails GetAuthentication
        void Restart(User_ptr UserPtr);

        //get authentication by sending http request to the web server
        bool GetAuthentication(User_ptr UserPtr);
};

#endif
