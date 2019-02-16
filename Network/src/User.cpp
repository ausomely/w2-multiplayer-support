#include "User.h"

User::User(tcp::socket socket_, Lobby& lobby_, boost::asio::io_service& io_serv)
    : socket(std::move(socket_)), lobby(lobby_), id(-1), io_service(io_serv), webServerSocket(io_serv)  {
//    ConnectToServer();
}

void User::InitializeSession() {
    currentSession = LoginSession::Instance(io_service);
    currentSession->Start(shared_from_this());
}

void User::ChangeSession(std::shared_ptr<Session> session) {
    currentSession = session;
    currentSession->Start(shared_from_this());
}

//Establish a connection to server with user's web socket
void User::ConnectToServer() {
     // Get a list of endpoints corresponding to the server name.
    tcp::resolver resolver(io_service);
    tcp::resolver::query query("ecs160.herokuapp.com", "http");
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

    // Try each endpoint until we successfully establish a connection.
    boost::asio::connect(webServerSocket, endpoint_iterator);
}

//logs out the user calling this function
//should be called whenver the user disconnects after login state
void User::Logout() {
    boost::asio::streambuf request;
    std::ostream request_stream(&request);
    ConnectToServer();
    //construct header
    request_stream << "DELETE /logout.json/ HTTP/1.1\r\n";
    request_stream << "Host:" << "ecs160.herokuapp.com." << "\r\n";
    request_stream << "User-Agent: C/1.0\r\n";
    request_stream << "Content-Type: application/json; charset=utf-8 \r\n";
    request_stream << "Authorization: Bearer " << jwt << "\r\n"; //present jwt
    request_stream << "Accept: */*\r\n";
    request_stream << "Content-Length: 0\r\n";
    request_stream << "Connection: close\r\n\r\n";  //NOTE THE Double line feed

    //write delete request to web server
    boost::asio::async_write(webServerSocket,  request,
        [this](boost::system::error_code err, std::size_t ) {
        if (!err) {
            std::cout << "No error on logout" << std::endl;
        }
    }); 
}
