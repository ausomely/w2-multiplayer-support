#include "AcceptedSession.h"
#include "User.h"
#include "Lobby.h"

std::shared_ptr< Session > AcceptedSession::DAcceptedSessionPointer;

std::shared_ptr< Session > AcceptedSession::Instance() {
    if(DAcceptedSessionPointer == nullptr) {
        DAcceptedSessionPointer = std::make_shared< AcceptedSession >(SPrivateSessionType());
    }
    return DAcceptedSessionPointer;
}


void AcceptedSession::DoRead(std::shared_ptr<User> UserPtr) {
    auto self(shared_from_this());
    bzero(UserPtr->data, MAX_BUFFER);
    UserPtr->socket.async_read_some(boost::asio::buffer(UserPtr->data, MAX_BUFFER),
        [UserPtr](boost::system::error_code err, std::size_t length) {

        if (!err) {

            // goes to FindGameSession if receives "Join"
            if(strcmp(UserPtr->data, "Join") == 0) {
                UserPtr->ChangeSession(FindGameSession::Instance());
            }

            // goes to HostGameSession if receives "Host"
            else if(strcmp(UserPtr->data, "Host") == 0) {
                UserPtr->ChangeSession(HostGameSession::Instance());
            }
        }

        //end of connection
        else if ((boost::asio::error::eof == err) ||
                (boost::asio::error::connection_reset == err)) {
            //find username in Lobby clients and remove data
            UserPtr->lobby.leave(UserPtr);
        }
    });
}

void AcceptedSession::DoWrite(std::shared_ptr<User> UserPtr) {
    auto self(shared_from_this());
    boost::asio::async_write(UserPtr->socket, boost::asio::buffer(UserPtr->data, MAX_BUFFER),
        [UserPtr](boost::system::error_code err, std::size_t ) {

        if (!err) {

        }
    });
 }

//start reading from connection
void AcceptedSession::Start(std::shared_ptr<User> UserPtr) {
    std::cout << UserPtr->name << " has joined accepted session" << std::endl;
    DoRead(UserPtr);
}
