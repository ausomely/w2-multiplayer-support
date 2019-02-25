#ifndef INGAMESESSION_H
#define INGAMESESSION_H

#include "Session.h"
#include "GameInfo.pb.h"

class User;

using boost::asio::ip::tcp;

class InGameSession : public Session, public std::enable_shared_from_this<InGameSession>
{
    /* Class to exchange information when players are in a game
       Information to exchange:
           Other player commands
    */

    private:
        struct SPrivateSessionType{};
    protected:
        static std::shared_ptr< Session > DInGameSessionPointer;
    public:
        explicit InGameSession(const SPrivateSessionType &key) {}

        static std::shared_ptr< Session > Instance();

        //read data from current session's socket
        void DoRead(std::shared_ptr<User> userPtr);

        //write data to server
        void DoWrite(std::shared_ptr<User> userPtr);

        //start reading from connection
        void Start(std::shared_ptr<User> userPtr);
};


#endif
