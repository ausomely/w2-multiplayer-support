#ifndef ACCEPTEDLOGINSESSION_H
#define ACCEPTEDLOGINSESSION_H

#include "Session.h"

class User;

using boost::asio::ip::tcp;

class AcceptedSession : public Session, public std::enable_shared_from_this<AcceptedSession>
{
    /* Class for forwarding client to FindGameSession or HostGameSession
       Goes to FindGameSession if receives "Join"
       Goes to HostGameSession if receives "Host"
    */
    private:
        struct SPrivateSessionType{};
    protected:
        static std::shared_ptr< Session > DAcceptedSessionPointer;
    public:
        explicit AcceptedSession(const SPrivateSessionType &key) {}

        static std::shared_ptr< Session > Instance();

        //read data from current session's socket
        void DoRead(std::shared_ptr<User> userPtr);

        //write data to server
        void DoWrite(std::shared_ptr<User> userPtr);

        //start reading from connection
        void Start(std::shared_ptr<User> userPtr);
};

#endif
