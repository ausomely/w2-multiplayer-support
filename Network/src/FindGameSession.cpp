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
            // goes back to AcceptedSession if receives "Back"
            if(strcmp(userPtr->data, "Back") == 0) {
                userPtr->ChangeSession(AcceptedSession::Instance());
                //DoWrite(userPtr);
            }

            // joins the room
            else {
                int index = std::stoi(std::string(userPtr->data));
                std::cout << userPtr->name << " wanted to join room " << index + 1 << std::endl;

                DoRead(userPtr);

                //userPtr->lobby.JoinRoom(userPtr, index);
                //userPtr->ChangeSession(InRoomSession::Instance());
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

void FindGameSession::DoWrite(std::shared_ptr<User> userPtr) {
    auto self(shared_from_this());
    RoomInfo::RoomInfoPackage roomList = userPtr->lobby.GetRoomList();

    bzero(userPtr->data, MAX_BUFFER);
    boost::asio::streambuf stream_buffer;
    std::ostream output_stream(&stream_buffer);
    roomList.SerializeToOstream(&output_stream);

    size_t length = roomList.ByteSizeLong();
    roomList.SerializeToArray(userPtr->data, length);

    boost::asio::async_write(userPtr->socket, stream_buffer,
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
