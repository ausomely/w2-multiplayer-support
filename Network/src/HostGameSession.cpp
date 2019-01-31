#include "HostGameSession.h"
#include "User.h"
#include "Lobby.h"

std::shared_ptr< Session > HostGameSession::DHostGameSessionPointer;

std::shared_ptr< Session > HostGameSession::Instance() {
    if(DHostGameSessionPointer == nullptr) {
        DHostGameSessionPointer = std::make_shared< HostGameSession >(SPrivateSessionType());
    }
    return DHostGameSessionPointer;
}


void HostGameSession::DoRead(std::shared_ptr<User> userPtr) {
    auto self(shared_from_this());
    bzero(userPtr->data, MAX_BUFFER);
    userPtr->socket.async_read_some(boost::asio::buffer(userPtr->data, MAX_BUFFER),
        [this, userPtr](boost::system::error_code err, std::size_t length) {

        if (!err) {
            // TODO : reading the game room information from client

            // Create the room object and add it to lobby
            std::shared_ptr<GameRoom> DRoom = std::make_shared<GameRoom> (userPtr, 8, "dummy_map");
            userPtr->currentRoom = DRoom;
            userPtr->lobby.AddRoom(DRoom);

            // incase we want to write something back
            DoWrite(userPtr);
        }

        //end of connection
        else if ((boost::asio::error::eof == err) ||
                (boost::asio::error::connection_reset == err)) {
            //find username in Lobby clients and remove data
            userPtr->lobby.leave(userPtr);
        }
    });
}

void HostGameSession::DoWrite(std::shared_ptr<User> userPtr) {
    auto self(shared_from_this());

    boost::asio::async_write(userPtr->socket, boost::asio::buffer(userPtr->data, MAX_BUFFER),
        [this, userPtr](boost::system::error_code err, std::size_t ) {
        //if no error, continue trying to read from socket
        if (!err) {
            DoRead(userPtr);
        }
    });
 }

//start reading from connection
void HostGameSession::Start(std::shared_ptr<User> userPtr) {
    std::cout << userPtr->name << " has joined Host Game session" << std::endl;
    DoRead(userPtr);
}
