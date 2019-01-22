#include "LoginSession.h"

/*std::string LoginSession::GetName() {
    return name;
}*/

void LoginSession::DoRead(User_ptr UserPtr)
{
    //currently assume there is only one message being sent to server
    //and that message being the username
    auto self(shared_from_this());
    bzero(UserPtr->data, MAX_BUFFER);
    UserPtr->socket.async_read_some(boost::asio::buffer(UserPtr->data, MAX_BUFFER),
        [this, UserPtr](boost::system::error_code err, std::size_t length) {
        //data to be processed
        if (!err) {
            //set session's username
            UserPtr->name = std::string(UserPtr->data);
            //add name to list of users
            UserPtr->lobby.join(UserPtr);
            DoWrite(UserPtr);
        }

        //end of connection
        else if ((boost::asio::error::eof == err) ||
                (boost::asio::error::connection_reset == err)) {
            //find username in Lobby clients and remove data
            UserPtr->lobby.leave(UserPtr);
        }
    });
}

void LoginSession::DoWrite(User_ptr UserPtr)
{
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
void LoginSession::Start(User_ptr UserPtr)
{
    DoRead(UserPtr);
}
