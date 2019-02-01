#ifndef INROOMSESSION_H
#define INROOMSESSION_H

#include "Session.h"

class User;

using boost::asio::ip::tcp;

class InRoomSession : public Session, public std::enable_shared_from_this<InRoomSession>
{
    /* Class to exchange information when players are in a room, waiting to start
       Information to exchange:
           current players' name
           add new players' name if joined
           remove players' name if left
    */

    private:
        struct SPrivateSessionType{};
    protected:
        static std::shared_ptr< Session > DInRoomSessionPointer;
    public:
        explicit InRoomSession(const SPrivateSessionType &key) {}

        static std::shared_ptr< Session > Instance();

        //read data from current session's socket
        void DoRead(std::shared_ptr<User> userPtr);

        //write data to server
        void DoWrite(std::shared_ptr<User> userPtr);

        //start reading from connection
        void Start(std::shared_ptr<User> userPtr);
};


#endif
