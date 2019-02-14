#include "User.h"

User::User(tcp::socket socket_, Lobby& lobby_, boost::asio::io_service& io_serv)
    : socket(std::move(socket_)), lobby(lobby_), id(-1), io_service(io_serv), resolver(io_service), query("ecs160.herokuapp.com", "http"), webServerSocket(io_serv)  {

      endpoint_iterator = resolver.resolve(query);
      boost::asio::connect(webServerSocket, endpoint_iterator);
}

void User::InitializeSession() {
    currentSession = LoginSession::Instance(io_service);
    currentSession->Start(shared_from_this());
}

void User::ChangeSession(std::shared_ptr<Session> session) {
    currentSession = session;
    currentSession->Start(shared_from_this());
}
