#include "LoginSession.h"
#include "AcceptedSession.h"
#include "LoginInfo.pb.h"

std::shared_ptr< Session > LoginSession::DLoginSessionPointer;

std::shared_ptr< Session > LoginSession::Instance() {
    if(DLoginSessionPointer == nullptr) {
        DLoginSessionPointer = std::make_shared< LoginSession >(SPrivateSessionType());
    }
    return DLoginSessionPointer;
}


void LoginSession::DoRead(User_ptr UserPtr) {
    auto self(shared_from_this());
    bzero(UserPtr->data, MAX_BUFFER);
    UserPtr->socket.async_read_some(boost::asio::buffer(UserPtr->data, MAX_BUFFER),
        [this, UserPtr](boost::system::error_code err, std::size_t length) {
        //data to be processed
        if (!err) {
            //TODO: parse login packet (username and password)
            //set session's username
            UserPtr->name = std::string(UserPtr->data);

            //TODO: send authentication request to web server

            //if authenticated
            if(GetAuthentication(UserPtr)) {
                //add name to list of users
                UserPtr->lobby.join(UserPtr);
                //write message to connected client and move to the next session
                DoWrite(UserPtr);
            }

            //else
            //send fail and continue reading as login session
            else {
                // just call restart to read data again from client
                Restart(UserPtr);
            }
        }

        //end of connection
        else if ((boost::asio::error::eof == err) ||
                (boost::asio::error::connection_reset == err)) {
            //find username in Lobby clients and remove data
            UserPtr->lobby.leave(UserPtr);
        }
    });
}

void LoginSession::DoWrite(User_ptr UserPtr) {
    auto self(shared_from_this());
    std::cout << "Client " << UserPtr->name << " has joined!" << std::endl;
    //print names of current connections and put in buffer
    size_t Length = UserPtr->lobby.PrepareUsersInfo(UserPtr->data);

    //write list of clients to socket
    boost::asio::async_write(UserPtr->socket, boost::asio::buffer(UserPtr->data, Length),
        [UserPtr](boost::system::error_code err, std::size_t ) {
        //if no error, move to the next session
        if (!err) {
            UserPtr->ChangeSession(AcceptedSession::Instance());
        }
    });
 }

//start reading from connection
void LoginSession::Start(User_ptr UserPtr) {
    DoRead(UserPtr);
}

//restart and read data again
void LoginSession::Restart(User_ptr UserPtr) {
    auto self(shared_from_this());
    char message[100] = "Your login information is wrong. Please try again\n";
    std::cout << "Client " << UserPtr->name << " failed to authenticate!" << std::endl;
    size_t Length = strlen(message);
    //write list of clients to socket
    boost::asio::async_write(UserPtr->socket, boost::asio::buffer(message, Length),
        [this, UserPtr](boost::system::error_code err, std::size_t ) {
        //if no error, continue trying to read from socket to get log in info
        if (!err) {
            DoRead(UserPtr);
        }
    });
}


//TO DO
bool LoginSession::GetAuthentication(User_ptr UserPtr) {
    // TO DO : get authentication / send http request
    return true;
}
