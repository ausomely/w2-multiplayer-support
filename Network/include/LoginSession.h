#ifndef LOGINSESSION_H
#define LOGINSESSION_H

#include "Session.h"

class User;

using boost::asio::ip::tcp;

class LoginSession : public Session, public std::enable_shared_from_this<LoginSession>
{
    /* Class for managing a single connection with a client
       Information to exchange:
           Login information:
               username and password
    */
    private:
        struct SPrivateSessionType{};
    protected:
        static std::shared_ptr< Session > DLoginSessionPointer;
    public:

        explicit LoginSession(const SPrivateSessionType &key);

        static std::shared_ptr< Session > Instance();

        //read data from current session's socket
        void DoRead(std::shared_ptr<User> userPtr);

        //write data to server
        void DoWrite(std::shared_ptr<User> userPtr);

        //start reading from connection
        void Start(std::shared_ptr<User> userPtr);

        //restart and read data from client again if fails GetAuthentication
        void Restart(std::shared_ptr<User> userPtr);

        //get authentication by sending http request to the web server
        void StartAuthentication(std::shared_ptr<User> userPtr);

        void FinishAuthentication(std::shared_ptr<User> userPtr);
};

#endif
