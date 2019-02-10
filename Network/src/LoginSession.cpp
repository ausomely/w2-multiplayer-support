#include "LoginSession.h"
#include "InGameSession.h"
#include "AcceptedSession.h"
#include "LoginInfo.pb.h"
#include "Lobby.h"
#include "User.h"

#include <sstream>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using boost::property_tree::ptree;
using boost::property_tree::read_json;
using boost::property_tree::write_json;
using boost::asio::ip::tcp;

std::shared_ptr< Session > LoginSession::DLoginSessionPointer;

std::shared_ptr< Session > LoginSession::Instance(boost::asio::io_service& io_serv) {
    if(DLoginSessionPointer == nullptr) {
        DLoginSessionPointer = std::make_shared< LoginSession >(SPrivateSessionType(), io_serv);
    }
    return DLoginSessionPointer;
}
/*
tcp::resolver resolver(io_service);
tcp::resolver::query query("ecs160.herokuapp.com", "http");
tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
*/

LoginSession::LoginSession(const SPrivateSessionType &key, boost::asio::io_service& io_serv) : io_service(io_serv), resolver(io_service), query("ecs160.herokuapp.com", "http"), sock(io_serv) {
    endpoint_iterator = resolver.resolve(query);
    boost::asio::connect(sock, endpoint_iterator);
}

void LoginSession::DoRead(std::shared_ptr<User> userPtr) {
    auto self(shared_from_this());
    bzero(userPtr->data, MAX_BUFFER);

    userPtr->socket.async_read_some(boost::asio::buffer(userPtr->data, MAX_BUFFER),
        [this, userPtr](boost::system::error_code err, std::size_t length) {
        //data to be processed
        if (!err) {

            //set session's username
            LoginInfo::Credential credential;

            credential.ParseFromString(userPtr->data);
            userPtr->name = credential.username();
            userPtr->password = credential.password();

            //testing if we got the password!
            std::cout << "Password for " << userPtr->name << " is " << userPtr->password << std::endl;

            //TODO: send authentication request to web server

            //if authenticated
            StartAuthentication(userPtr);
/*
            if(GetAuthentication(userPtr)) {
                //add name to list of users
                userPtr->lobby.join(userPtr);
                //write message to connected client and move to the next session
                DoWrite(userPtr);
            }

            //else
            //send fail and continue reading as login session
            else {
                // just call restart to read data again from client
                Restart(userPtr);
            }
            */
        }

        //end of connection
        else if ((boost::asio::error::eof == err) ||
                (boost::asio::error::connection_reset == err)) {
            //find username in Lobby clients and remove data
            userPtr->lobby.leave(userPtr);
        }
    });
}

void LoginSession::DoWrite(std::shared_ptr<User> userPtr) {
    auto self(shared_from_this());
    std::cout << "Client " << userPtr->name << " has joined!" << std::endl;
    //print names of current connections and put in buffer
    userPtr->lobby.PrepareUsersInfo(userPtr->data);
    //write list of clients to socket
    boost::asio::async_write(userPtr->socket, boost::asio::buffer(userPtr->data, MAX_BUFFER),
        [userPtr](boost::system::error_code err, std::size_t ) {
        //if no error, move to the next session
        if (!err) {
            userPtr->ChangeSession(AcceptedSession::Instance());
        }
    });
 }

//start reading from connection
void LoginSession::Start(std::shared_ptr<User> userPtr) {
    DoRead(userPtr);
}

//restart and read data again
void LoginSession::Restart(std::shared_ptr<User> userPtr) {
    auto self(shared_from_this());
    std::string message = "Your login information is wrong. Please try again\n";
    std::cout << "Client " << userPtr->name << " failed to authenticate!" << std::endl;

    //write list of clients to socket
    boost::asio::async_write(userPtr->socket, boost::asio::buffer(message.c_str(), MAX_BUFFER),
        [userPtr](boost::system::error_code err, std::size_t ) {
        //if no error, continue trying to read from socket to get log in info
        if (!err) {

        }
    });
}


void LoginSession::StartAuthentication(std::shared_ptr<User> userPtr){
    boost::asio::streambuf request;
    std::ostream request_stream(&request);

    ptree root, info;

    info.put("email", userPtr->name + "@ucdavis.edu");
    info.put("password", userPtr->password);
    root.put_child("user", info);


    std::ostringstream buf;
    write_json (buf, root, true);
    std::string json = buf.str();

    request_stream << "POST /login.json/ HTTP/1.1\r\n";
    request_stream << "Host:" << "ecs160.herokuapp.com." << "\r\n";
    request_stream << "User-Agent: C/1.0\r\n";
    request_stream << "Content-Type: application/json; charset=utf-8 \r\n";
    request_stream << "Authorization: Bearer \r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Content-Length: " << json.length() << "\r\n";
    request_stream << "Connection: close\r\n\r\n";  //NOTE THE Double line feed
    request_stream << json;


    boost::asio::async_write(sock,  request,
        [this, userPtr](boost::system::error_code err, std::size_t ) {
        //if no error, continue trying to read from socket
        if (!err) {
            std::cout << "No error" << std::endl;
            FinishAuthentication(userPtr);
        }
        //FinishAuthentication(userPtr);
    });

}


void LoginSession::FinishAuthentication(std::shared_ptr<User> userPtr){

  boost::asio::async_read_until(sock, response, "\r\n",
      [this, userPtr](boost::system::error_code err, std::size_t length) {
      std::cout << "Reading" << std::endl;
      if (!err) {
          std::istream response_stream(&response);
          std::string http_version;
          response_stream >> http_version;
          unsigned int status_code;
          response_stream >> status_code;
          std::string status_message;

          std::getline(response_stream, status_message);
          if (!response_stream || http_version.substr(0, 5) != "HTTP/")
          {
            std::cout << "Invalid response\n";
            Restart(userPtr);
          }
          if (status_code != 200)
          {
            std::cout << "Response returned with status code " << status_code << "\n";
            Restart(userPtr);
          }

          //write message to connected client and move to the next session
          userPtr->lobby.join(userPtr);
          DoWrite(userPtr);
          std::cout << "authenticated\n";
      }
  });

}

bool LoginSession::GetAuthentication(std::shared_ptr<User> userPtr) {

    boost::asio::io_service io_service;
      // Get a list of endpoints corresponding to the server name.
    tcp::resolver resolver(io_service);
    tcp::resolver::query query("ecs160.herokuapp.com", "http");
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

    // Try each endpoint until we successfully establish a connection.
    tcp::socket socket(io_service);
    boost::asio::connect(socket, endpoint_iterator);

    // Form the request. We specify the "Connection: close" header so that the
    // server will close the socket after transmitting the response. This will
    // allow us to treat all data up until the EOF as the content.
    boost::asio::streambuf request;
    std::ostream request_stream(&request);

    ptree root, info;

    info.put("email", userPtr->name + "@ucdavis.edu");
    info.put("password", userPtr->password);
    root.put_child("user", info);


    std::ostringstream buf;
    write_json (buf, root, true);
    std::string json = buf.str();

    request_stream << "POST /login.json/ HTTP/1.1\r\n";
    request_stream << "Host:" << "ecs160.herokuapp.com." << "\r\n";
    request_stream << "User-Agent: C/1.0\r\n";
    request_stream << "Content-Type: application/json; charset=utf-8 \r\n";
    request_stream << "Authorization: Bearer \r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Content-Length: " << json.length() << "\r\n";
    request_stream << "Connection: close\r\n\r\n";  //NOTE THE Double line feed
    request_stream << json;

    // Send the request.
    boost::asio::write(socket, request);

    boost::asio::streambuf response;
    boost::asio::read_until(socket, response, "\r\n");

    // Check that response is OK.
    std::istream response_stream(&response);
    std::string http_version;
    response_stream >> http_version;
    unsigned int status_code;
    response_stream >> status_code;
    std::string status_message;

    std::getline(response_stream, status_message);
    if (!response_stream || http_version.substr(0, 5) != "HTTP/")
    {
      std::cout << "Invalid response\n";
      return false;
    }
    if (status_code != 200)
    {
      std::cout << "Response returned with status code " << status_code << "\n";
      return false;
    }

    // Read the response headers, which are terminated by a blank line.
  //  boost::asio::read_until(socket, response, "\r\n\r\n");

    return true;
}
