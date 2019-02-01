#include "FindGameSession.h"
#include "RoomInfo.pb.h"
#include "User.h"
#include "Lobby.h"

std::shared_ptr< Session > FindGameSession::DFindGameSessionPointer;

std::shared_ptr< Session > FindGameSession::Instance() {
    if(DFindGameSessionPointer == nullptr) {
        DFindGameSessionPointer = std::make_shared< FindGameSession >(SPrivateSessionType());
    }
    return DFindGameSessionPointer;
}

void FindGameSession::DoRead(std::shared_ptr<User> userPtr) {
    auto self(shared_from_this());
    bzero(userPtr->data, MAX_BUFFER);
    userPtr->socket.async_read_some(boost::asio::buffer(userPtr->data, MAX_BUFFER),
        [this, userPtr](boost::system::error_code err, std::size_t length) {

        if (!err) {
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

void FindGameSession::DoWrite(std::shared_ptr<User> userPtr) {
    auto self(shared_from_this());
    bzero(userPtr->data, MAX_BUFFER);
    RoomInfo::RoomInfoPackage roomList = userPtr->lobby.GetRoomList();
    size_t size = roomList.ByteSizeLong();
    roomList.SerializeToArray(userPtr->data, size);
    boost::asio::async_write(userPtr->socket, boost::asio::buffer(userPtr->data, MAX_BUFFER),
        [this, userPtr](boost::system::error_code err, std::size_t ) {
        //if no error, continue trying to read input from client
        if (!err) {
            DoRead(userPtr);
        }
    });
 }

//start reading from connection
void FindGameSession::Start(std::shared_ptr<User> userPtr) {
    std::cout << userPtr->name << " has joined Find Game session" << std::endl;

    // send game list information
    DoWrite(userPtr);
}
