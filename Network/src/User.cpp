#include "User.h"
#include <sstream>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using boost::property_tree::ptree;
using boost::property_tree::read_json;
using boost::property_tree::write_json;

User::User(tcp::socket socket_, Lobby& lobby_, boost::asio::io_service& io_serv)
    : io_service(io_serv), socket(std::move(socket_)), webServerSocket(io_serv), lobby(lobby_), id(-1) {
}

void User::InitializeSession() {
    currentSession = LoginSession::Instance();
    currentSession->Start(shared_from_this());
}

void User::ChangeSession(std::shared_ptr<Session> session) {
    currentSession = session;
    currentSession->Start(shared_from_this());
}

// tell the client its done with updating info
void User::SendFinish() {
    std::string message = "Finish";
    boost::asio::async_write(socket, boost::asio::buffer(message.c_str(), MAX_BUFFER),
        [](boost::system::error_code err, std::size_t ) {
        if (!err) {
        }
    });
}

void User::WriteMatchResult() {
      // Function to write results to the web server after the match has ended
      // Currently there is a problem with the response -- whether or not this is a problem here or with the web server is unknown

      // Connect to server
      ConnectToServer();

      // Create the request stream
      boost::asio::streambuf request;
      std::ostream request_stream(&request);
      std::ostringstream buf;

      // Create JSON to send
      ptree root;
      ptree participants;
      std::string IDs;

      for(auto &It: currentRoom.lock()->players) {
          ptree id;
          id.put<int>("", It->uid);
          participants.push_back(std::make_pair("", id));
      }
      root.add_child("participants", participants);
      root.put<int>("winner_id", uid);

      write_json (buf, root, true);
      std::string json = buf.str();
      std::cout << json << std::endl;

      // Write header
      request_stream << "POST /match.json/ HTTP/1.1\r\n";
      request_stream << "Host:" << "ecs160.herokuapp.com." << "\r\n";
      request_stream << "User-Agent: C/1.0\r\n";
      request_stream << "Content-Type: application/json; charset=utf-8 \r\n";
      request_stream << "Authorization: Bearer " << jwt << "\r\n";
      request_stream << "Accept: */*\r\n";
      request_stream << "Content-Length: " << json.length() << "\r\n";
      request_stream << "Connection: close\r\n\r\n";  //NOTE THE Double line feed
      request_stream << json;
      // Write to socket
      boost::asio::async_write(webServerSocket, request,
          [this](boost::system::error_code err, std::size_t ) {
          //if no error, continue trying to read from socket
          if (!err) {
              std::cout << "No error with writing result to web server" << std::endl;
              ReadMatchResult();
          }
      });
}

void User::ReadMatchResult() {
    // Reads the response from the web server about the match results
    response.consume(response.size());
    boost::asio::async_read_until(webServerSocket, response, "\r\n",
        [this](boost::system::error_code err, std::size_t length) {

        if (!err) {
            std::istream response_stream(&this->response);
            std::string http_version;
            response_stream >> http_version;
            unsigned int status_code;
            response_stream >> status_code;
            std::string status_message;
            std::getline(response_stream, status_message);
            if (!response_stream || http_version.substr(0, 5) != "HTTP/")
            {
                std::cout << "Invalid response\n";
            }
            else if (status_code != 201)
            {
                std::cout << "Response returned with status code " << status_code << "\n";
            }
            else {
                std::cout << "Posted match result successfully" << std::endl;
            }

        }
        else {
            std::cout << "Error reading results response" << std::endl;
        }

        // Close the connection
        webServerSocket.close();
    });

}

void User::StartPostMap(std::string input){

}

void User::FinishPostMap(){
}

//Establish a connection to server with user's web socket
void User::ConnectToServer() {
     // Get a list of endpoints corresponding to the server name.
    tcp::resolver resolver(io_service);
    tcp::resolver::query query("ecs160.herokuapp.com", "http");
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

    // This should be async_connect
    // Try each endpoint until we successfully establish a connection.
    boost::asio::connect(webServerSocket, endpoint_iterator);
}

//logs out the user calling this function
//should be called whenver the user disconnects after login state
void User::Logout() {
    boost::asio::streambuf request;
    std::ostream request_stream(&request);
    //connect to web server
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
    boost::asio::async_write(webServerSocket, request,
        [](boost::system::error_code err, std::size_t ) {
        if (!err) {
            std::cout << "No error on logout" << std::endl;
        }
    });
}
