#include "LoginSession.h"

std::string LoginSession::GetName() {
    return name;
}

void LoginSession::DoRead()
{
    //currently assume there is only one message being sent to server
    //and that message being the username
    auto self(shared_from_this());
    bzero(data, MAX_BUFFER);
    socket.async_read_some(boost::asio::buffer(data, MAX_BUFFER),
        [this, self](boost::system::error_code err, std::size_t length) {
        //data to be processed
        if (!err) {
            //set session's username
            this->name = std::string(data);
            //add name to list of users
            lobby.join(self);
            DoWrite();
        }

        //end of connection
        else if ((boost::asio::error::eof == err) ||
                (boost::asio::error::connection_reset == err)) {
            //find username in Lobby clients and remove data
            lobby.leave(self);
        }
    });
}

void LoginSession::DoWrite()
{
    auto self(shared_from_this());
    std::cout << "Client " << name << " has joined!" << std::endl;
    //print names of current connections and put in buffer
    size_t Length = lobby.PrepareUsersInfo(data);

    //write list of clients to socket
    boost::asio::async_write(socket, boost::asio::buffer(data, Length),
        [this, self](boost::system::error_code err, std::size_t ) {
        //if no error, continue trying to read from socket
        if (!err) {
            DoRead();
        }
    });
 }

//start reading from connection
void LoginSession::Start()
{
    DoRead();
}
