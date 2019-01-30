#ifndef FINDGAMESESSION_H
#define FINDGAMESESSION_H

#include "Session.h"

class User;

using boost::asio::ip::tcp;

class FindGameSession : public Session, public std::enable_shared_from_this<FindGameSession>
{
    // Class for managing information exchange between client and server when the client wants to find a game room
    // information to exchange: list of game room info
    // game room info includes: host client's name, current number of player, maximum number of allowed player, map name
    private:
        struct SPrivateSessionType{};
    protected:
        static std::shared_ptr< Session > DFindGameSessionPointer;
    public:
        explicit FindGameSession(const SPrivateSessionType &key) {}

        static std::shared_ptr< Session > Instance();

        //read data from current session's socket
        void DoRead(std::shared_ptr<User>  UserPtr);

        //write data to server
        void DoWrite(std::shared_ptr<User>  UserPtr);

        //start reading from connection
        void Start(std::shared_ptr<User>  UserPtr);
};

#endif
