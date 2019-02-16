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


void AcceptedSession::DoRead(std::shared_ptr<User> userPtr) {
    auto self(shared_from_this());
    bzero(userPtr->data, MAX_BUFFER);
    userPtr->socket.async_read_some(boost::asio::buffer(userPtr->data, MAX_BUFFER),
        [userPtr](boost::system::error_code err, std::size_t length) {

        if (!err) {

            // goes to FindGameSession if receives "Join"
            if(strcmp(userPtr->data, "Join") == 0) {
                userPtr->ChangeSession(FindGameSession::Instance());
            }

            // goes to HostGameSession if receives "Host"
            else if(strcmp(userPtr->data, "Host") == 0) {
                userPtr->ChangeSession(HostGameSession::Instance());
            }
        }

        //end of connection
        else if ((boost::asio::error::eof == err) ||
                (boost::asio::error::connection_reset == err)) {
            //find username in Lobby clients and remove data
            userPtr->Logout();
            userPtr->lobby.leave(userPtr);
        }
    });
}

void AcceptedSession::DoWrite(std::shared_ptr<User> userPtr) {
    auto self(shared_from_this());
    boost::asio::async_write(userPtr->socket, boost::asio::buffer(userPtr->data, MAX_BUFFER),
        [userPtr](boost::system::error_code err, std::size_t ) {

        if (!err) {

        }
    });
 }

//start reading from connection
void AcceptedSession::Start(std::shared_ptr<User> userPtr) {
    std::cout << userPtr->name << " has joined Accepted session" << std::endl;
    DoRead(userPtr);
}
