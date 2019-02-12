#include "InRoomSession.h"
#include "User.h"
#include "Lobby.h"
#include <fstream>

std::shared_ptr< Session > InRoomSession::DInRoomSessionPointer;

std::shared_ptr< Session > InRoomSession::Instance() {
    if(DInRoomSessionPointer == nullptr) {
        DInRoomSessionPointer = std::make_shared< InRoomSession >(SPrivateSessionType());
    }
    return DInRoomSessionPointer;
}

void InRoomSession::DoRead(std::shared_ptr<User> userPtr) {
    auto self(shared_from_this());
    bzero(userPtr->data, MAX_BUFFER);
    userPtr->socket.async_read_some(boost::asio::buffer(userPtr->data, MAX_BUFFER),
        [userPtr](boost::system::error_code err, std::size_t length) {

        if (!err) {
          // goes to InGameSession if receives "Test"
            if(strcmp(userPtr->data, "Test") == 0) {
                userPtr->ChangeSession(InGameSession::Instance());
            }

            else if(strcmp(userPtr->data, "Leave") == 0) {
                userPtr->SendFinish();
                userPtr->currentRoom.lock()->leave(userPtr);
                userPtr->ChangeSession(AcceptedSession::Instance());
            }

        }

        //end of connection
        else if ((boost::asio::error::eof == err) ||
                (boost::asio::error::connection_reset == err)) {
            //find username in Lobby clients and remove data
            userPtr->lobby.leave(userPtr);
        }
    });
}

void InRoomSession::DoWrite(std::shared_ptr<User> userPtr) {
    auto self(shared_from_this());
    bzero(userPtr->data, MAX_BUFFER);
    //userPtr->currentRoom.lock()->SetData(userPtr->data);
    //write game package to socket
    boost::asio::async_write(userPtr->socket, boost::asio::buffer(userPtr->data, MAX_BUFFER),
        [this, userPtr](boost::system::error_code err, std::size_t ) {
        //if no error, continue trying to read from socket
        if (!err) {
            DoRead(userPtr);
        }

        //leave the room if disconnected or error comes up
        else {

        }
    });
 }

//start reading from connection
void InRoomSession::Start(std::shared_ptr<User> userPtr) {
    std::cout << userPtr->name << " has joined in room session" << std::endl;
    DoRead(userPtr);
}
