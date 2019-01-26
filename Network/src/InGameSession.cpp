#include "InGameSession.h"
#include "User.h"
#include "Lobby.h"

std::shared_ptr< Session > InGameSession::DInGameSessionPointer;

std::shared_ptr< Session > InGameSession::Instance() {
    if(DInGameSessionPointer == nullptr) {
        DInGameSessionPointer = std::make_shared< InGameSession >(SPrivateSessionType());
    }
    return DInGameSessionPointer;
}


void InGameSession::DoRead(std::shared_ptr<User>  UserPtr) {
    auto self(shared_from_this());
    bzero(UserPtr->data, MAX_BUFFER);
    UserPtr->socket.async_read_some(boost::asio::buffer(UserPtr->data, MAX_BUFFER),
        [this, UserPtr](boost::system::error_code err, std::size_t length) {
        //TODO: determine how to handle just authenticated session
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

void InGameSession::DoWrite(std::shared_ptr<User>  UserPtr) {
    auto self(shared_from_this());
    std::cout << "Client " << UserPtr->name << " has joined!" << std::endl;
    //print names of current connections and put in buffer
    UserPtr->lobby.PrepareUsersInfo(UserPtr->data);

    //write list of clients to socket
    boost::asio::async_write(UserPtr->socket, boost::asio::buffer(UserPtr->data, MAX_BUFFER),
        [this, UserPtr](boost::system::error_code err, std::size_t ) {
        //if no error, continue trying to read from socket
        if (!err) {
            DoRead(UserPtr);
        }
    });
 }

//start reading from connection
void InGameSession::Start(std::shared_ptr<User>  UserPtr) {
    std::cout << UserPtr->name << " has joined in game session" << std::endl;
    DoRead(UserPtr);
}
