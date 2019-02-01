#include "InGameSession.h"
#include "GameInfo.pb.h"
#include "User.h"
#include "Lobby.h"
#include <fstream>

std::shared_ptr< Session > InGameSession::DInGameSessionPointer;

std::shared_ptr< Session > InGameSession::Instance() {
    if(DInGameSessionPointer == nullptr) {
        DInGameSessionPointer = std::make_shared< InGameSession >(SPrivateSessionType());
    }
    return DInGameSessionPointer;
}

void InGameSession::DoRead(std::shared_ptr<User> userPtr) {
    auto self(shared_from_this());
    bzero(userPtr->data, MAX_BUFFER);
    userPtr->socket.async_read_some(boost::asio::buffer(userPtr->data, MAX_BUFFER),
        [this, userPtr](boost::system::error_code err, std::size_t length) {

        if (!err) {
            GameInfo::PlayerCommandRequest playerCommandRequest;
            playerCommandRequest.ParseFromArray(userPtr->data, length);

            std::ofstream outfile;
            outfile.open("RemoteStreamCommand.bin", std::ios_base::app | std::ios::binary);

            playerCommandRequest.SerializeToOstream(&outfile);

            //outfile << playerCommandRequest.DebugString();
            outfile.close();
            userPtr->currentRoom.lock()->SetPlayerComand(playerCommandRequest, userPtr->id);
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

void InGameSession::DoWrite(std::shared_ptr<User> userPtr) {
    auto self(shared_from_this());
    bzero(userPtr->data, MAX_BUFFER);
    userPtr->currentRoom.lock()->SetData(userPtr->data);
    //write game package to socket
    boost::asio::async_write(userPtr->socket, boost::asio::buffer(userPtr->data, MAX_BUFFER),
        [this, userPtr](boost::system::error_code err, std::size_t ) {
        //if no error, continue trying to read from socket
        if (!err) {
            DoRead(userPtr);
        }

        //leave the room if disconnected or error comes up
        else {
            userPtr->currentRoom.lock()->leave(userPtr);
        }
    });
 }

//start reading from connection
void InGameSession::Start(std::shared_ptr<User> userPtr) {
    std::cout << userPtr->name << " has joined in game session" << std::endl;
    DoRead(userPtr);
}
