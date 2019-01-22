#include "Server.h"

Server::Server(boost::asio::io_service& io_service, short port)
    : acceptor(io_service, tcp::endpoint(tcp::v4(), port)), socket(io_service) {
    //listen for new connections
    DoAccept();
}

void Server::DoAccept() {
    acceptor.async_accept(socket,
        [this](boost::system::error_code err) {
        if (!err) {
            //accept new connection and create new Session for it
            std::make_shared<User>(std::move(socket), lobby)->InitializeSession();
        }
        //continue to listen for new connections
        DoAccept();
    });
}
