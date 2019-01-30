#ifndef HOSTGAMESESSION_H
#define HOSTGAMESESSION_H

#include "Session.h"

class User;

using boost::asio::ip::tcp;

class HostGameSession : public Session, public std::enable_shared_from_this<HostGameSession>
{
    // Class for managing information exchange between client and server when the client creates a game room
    // information to exchange: map name, maximum allowed number of player, name of the host client
    private:
        struct SPrivateSessionType{};
    protected:
        static std::shared_ptr< Session > DHostGameSessionPointer;
    public:
        explicit HostGameSession(const SPrivateSessionType &key) {}

        static std::shared_ptr< Session > Instance();

        //read data from current session's socket
        void DoRead(std::shared_ptr<User>  UserPtr);

        //write data to server
        void DoWrite(std::shared_ptr<User>  UserPtr);

        //start reading from connection
        void Start(std::shared_ptr<User>  UserPtr);
};

#endif
