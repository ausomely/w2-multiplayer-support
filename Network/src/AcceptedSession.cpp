#include "AcceptedSession.h"
#include "User.h"
#include "GameInfo.pb.h"
#include "Lobby.h"
#include <fstream>
#include <google/protobuf/text_format.h>
using namespace google::protobuf;

std::shared_ptr< Session > AcceptedSession::DAcceptedSessionPointer;

std::shared_ptr< Session > AcceptedSession::Instance() {
    if(DAcceptedSessionPointer == nullptr) {
        DAcceptedSessionPointer = std::make_shared< AcceptedSession >(SPrivateSessionType());
    }
    return DAcceptedSessionPointer;
}


void AcceptedSession::DoRead(std::shared_ptr<User>  UserPtr) {
    auto self(shared_from_this());
    bzero(UserPtr->data, MAX_BUFFER);
    UserPtr->socket.async_read_some(boost::asio::buffer(UserPtr->data, MAX_BUFFER),
        [this, UserPtr](boost::system::error_code err, std::size_t length) {
        //TODO: determine how to handle just authenticated session
        if (!err) {
            GameInfo::PlayerCommandRequest playerCommandRequest;
            playerCommandRequest.ParseFromArray(UserPtr->data,length);

            std::ofstream outfile;
            outfile.open("RemoteStreamCommand.bin", std::ios_base::app | std::ios::binary);

            playerCommandRequest.SerializeToOstream(&outfile);

            //outfile << playerCommandRequest.DebugString();
            outfile.close();
            DoRead(UserPtr);
        }

        //end of connection
        else if ((boost::asio::error::eof == err) ||
                (boost::asio::error::connection_reset == err)) {
            //find username in Lobby clients and remove data
            UserPtr->lobby.leave(UserPtr);
        }
    });
}

void AcceptedSession::DoWrite(std::shared_ptr<User>  UserPtr) {
    auto self(shared_from_this());
    std::cout << "Client " << UserPtr->name << " has joined!" << std::endl;
    //print names of current connections and put in buffer
    size_t Length = UserPtr->lobby.PrepareUsersInfo(UserPtr->data);

    //write list of clients to socket
    boost::asio::async_write(UserPtr->socket, boost::asio::buffer(UserPtr->data, Length),
        [this, UserPtr](boost::system::error_code err, std::size_t ) {
        //if no error, continue trying to read from socket
        if (!err) {
            DoRead(UserPtr);
        }
    });
 }

//start reading from connection
void AcceptedSession::Start(std::shared_ptr<User>  UserPtr) {
    std::cout << UserPtr->name << " has joined accepted session" << std::endl;
    DoRead(UserPtr);
}
