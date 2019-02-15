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


            /*std::ofstream outfile;
            outfile.open("RemoteStreamCommand.bin", std::ios_base::app | std::ios::binary);

            playerCommandRequest.SerializeToOstream(&outfile);*/

            //outfile.close();
            userPtr->currentRoom.lock()->SetPlayerComand(playerCommandRequest, (userPtr->id) - 1);
            //WriteToAll(userPtr, playerCommandRequest);
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

 void InGameSession::WriteToAll(std::shared_ptr<User> userPtr, GameInfo::PlayerCommandRequest playerCommandRequest) {

     for(auto i : userPtr->lobby.users){
         if(i != userPtr){   //Don't need to write own actions to self.
         boost::system::error_code err;
         boost::asio::streambuf stream_buffer;
         std::ostream output_stream(&stream_buffer);
         playerCommandRequest.SerializeToOstream(&output_stream);

         boost::asio::write(i->socket,stream_buffer,err);

          if(err) {
             std::cerr << "ERROR writing AHA" << std::endl;
             return;
          }
        // std::cout << "Close your eyes!\n\n";
         }
     }

     DoRead(userPtr);
 }


//start reading from connection
void InGameSession::Start(std::shared_ptr<User> userPtr) {
    std::cout << userPtr->name << " has joined in game session" << std::endl;
    // set tcp_no_delay for sending data
    boost::asio::ip::tcp::no_delay option(true);
    userPtr->socket.set_option(option);
    DoRead(userPtr);
}
