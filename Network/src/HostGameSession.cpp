#include "HostGameSession.h"
#include "RoomInfo.pb.h"
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
            // goes back to AcceptedSession if receives "Back"
            if(strcmp(userPtr->data, "Back") == 0) {
                userPtr->lobby.RemoveRoom(userPtr->currentRoom.lock());
                userPtr->ChangeSession(AcceptedSession::Instance());
            }

            else {
                RoomInfo::RoomInformation roomInfo;
                roomInfo.ParseFromArray(userPtr->data, length);

                std::cout << roomInfo.DebugString() << std::endl;

                // Create the room object and add it to lobby
                std::shared_ptr<GameRoom> DRoom = std::make_shared<GameRoom> (userPtr, roomInfo);
                userPtr->currentRoom = DRoom;
                userPtr->lobby.AddRoom(DRoom);

                // send notification to people that in FindGameSession
                DoWrite(userPtr);

                // move to in room session
                userPtr->ChangeSession(InRoomSession::Instance());
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

void HostGameSession::DoWrite(std::shared_ptr<User> userPtr) {
    auto self(shared_from_this());
    boost::asio::streambuf stream_buffer;
    std::ostream output_stream(&stream_buffer);
    RoomInfo::RoomInfoPackage roomList = userPtr->lobby.GetRoomList();
    roomList.SerializeToOstream(&output_stream);

    for(auto &It : userPtr->lobby.users) {
        // sending notification to those in find game session to update room list
        if(It->currentSession == FindGameSession::Instance()) {
            boost::asio::async_write(It->socket, stream_buffer,
                [It](boost::system::error_code err, std::size_t ) {

                if (!err) {

                }
            });
        }
     }
 }

//start reading from connection
void HostGameSession::Start(std::shared_ptr<User> userPtr) {
    std::cout << userPtr->name << " has joined Host Game session" << std::endl;
    DoRead(userPtr);
}
