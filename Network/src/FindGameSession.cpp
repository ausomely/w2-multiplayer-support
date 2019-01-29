#include "FindGameSession.h"
#include "User.h"
#include "Lobby.h"

std::shared_ptr< Session > FindGameSession::DFindGameSessionPointer;

std::shared_ptr< Session > FindGameSession::Instance() {
    if(DFindGameSessionPointer == nullptr) {
        DFindGameSessionPointer = std::make_shared< FindGameSession >(SPrivateSessionType());
    }
    return DFindGameSessionPointer;
}


void FindGameSession::DoRead(std::shared_ptr<User>  UserPtr) {
    auto self(shared_from_this());
    bzero(UserPtr->data, MAX_BUFFER);
    UserPtr->socket.async_read_some(boost::asio::buffer(UserPtr->data, MAX_BUFFER),
        [this, UserPtr](boost::system::error_code err, std::size_t length) {

        if (!err) {
            DoWrite(UserPtr);
        }

        //end of connection
        else if ((boost::asio::error::eof == err) ||
                (boost::asio::error::connection_reset == err)) {
            //find username in Lobby clients and remove data
            UserPtr->lobby.leave(UserPtr);
        }
    });
}

void FindGameSession::DoWrite(std::shared_ptr<User>  UserPtr) {
    auto self(shared_from_this());

    boost::asio::async_write(UserPtr->socket, boost::asio::buffer(UserPtr->data, MAX_BUFFER),
        [this, UserPtr](boost::system::error_code err, std::size_t ) {
        //if no error, continue trying to read from socket
        if (!err) {
            DoRead(UserPtr);
        }
    });
 }

//start reading from connection
void FindGameSession::Start(std::shared_ptr<User>  UserPtr) {
    std::cout << UserPtr->name << " has joined Find Game session" << std::endl;
    DoRead(UserPtr);
}
