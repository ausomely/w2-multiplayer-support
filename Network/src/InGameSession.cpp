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
            // game over, go back to room
            if(strcmp(userPtr->data, "Back") == 0) {
                userPtr->currentRoom.lock()->SendRoomInfo(userPtr);
                userPtr->currentRoom.lock()->UpdateRoomList(userPtr);
                userPtr->ChangeSession(InRoomSession::Instance());
            }
            // end the game
            else if(strcmp(userPtr->data, "Win") == 0 || strcmp(userPtr->data, "Lose") == 0) {
                userPtr->currentRoom.lock()->IncreaseEndNum();
                if(strcmp(userPtr->data, "Win") == 0) {
                    userPtr->WriteMatchResult();
                }
                DoRead(userPtr);
            }

            // leave the room
            else if(strcmp(userPtr->data, "Leave") == 0) {
                boost::asio::ip::tcp::no_delay option(false);
                userPtr->socket.set_option(option);
                // leave the room
                userPtr->currentRoom.lock()->leave(userPtr);
                // go back to accepted session
                userPtr->ChangeSession(AcceptedSession::Instance());
            }

            // game over, leave and log out
            else if(strcmp(userPtr->data, "Exit") == 0) {
                // leave the room
                userPtr->currentRoom.lock()->leave(userPtr);
                // find username in Lobby clients and remove data
                userPtr->Logout();
                userPtr->lobby.leave(userPtr);
            }
            else {
                GameInfo::PlayerCommandRequest playerCommandRequest;
                playerCommandRequest.ParseFromArray(userPtr->data, length);
                userPtr->currentRoom.lock()->SetPlayerComand(playerCommandRequest, (userPtr->playerNum) - 1);
                DoRead(userPtr);
            }
        }

        //end of connection
        else if ((boost::asio::error::eof == err) ||
                (boost::asio::error::connection_reset == err)) {
            // leave the room
            userPtr->currentRoom.lock()->leave(userPtr);
            // find username in Lobby clients and remove data
            userPtr->Logout();
            userPtr->lobby.leave(userPtr);
        }
    });
}

void InGameSession::DoWrite(std::shared_ptr<User> userPtr) {
    auto self(shared_from_this());
    GameInfo::PlayerCommandPackage playerCommandPackage = userPtr->currentRoom.lock()->GetPlayerCommandPackage();

    boost::asio::streambuf stream_buffer;
    std::ostream output_stream(&stream_buffer);
    playerCommandPackage.SerializeToOstream(&output_stream);

    //write game package to socket
    boost::asio::async_write(userPtr->socket, stream_buffer,
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
