#include "User.h"
#include <sstream>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/remove_whitespace.hpp>
#include <algorithm>


using boost::property_tree::ptree;
using boost::property_tree::read_json;
using boost::property_tree::write_json;


User::User(tcp::socket socket_, Lobby& lobby_, boost::asio::io_service& io_serv)
    : socket(std::move(socket_)), lobby(lobby_), id(-1), io_service(io_serv), resolver(io_service), query("ecs160.herokuapp.com", "http"), webServerSocket(io_serv)  {
      std::cout << "connecting" << std::endl;
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


void User::WriteMatchResult(bool win){

      boost::asio::streambuf request;
      std::ostream request_stream(&request);
      std::ostringstream buf;
      ptree root;

      root.put("winner", win);

      write_json (buf, root, true);
      std::string json = buf.str();

      std::cout << json << std::endl;

      request_stream << "POST /match.json/ HTTP/1.1\r\n";
      request_stream << "Host:" << "ecs160.herokuapp.com." << "\r\n";
      request_stream << "User-Agent: C/1.0\r\n";
      request_stream << "Content-Type: application/json; charset=utf-8 \r\n";
      request_stream << "Authorization: Bearer \r\n";
      request_stream << "Accept: */*\r\n";
      request_stream << "Content-Length: " << json.length() << "\r\n";
      request_stream << "Connection: close\r\n\r\n";  //NOTE THE Double line feed
      request_stream << json;

      boost::asio::async_write(webServerSocket,  request,
          [this](boost::system::error_code err, std::size_t ) {
          //if no error, continue trying to read from socket
          if (!err) {
            std::cout << "No error with writing result to web server" << std::endl;
            ReadMatchResult();
          }
      });
}

void User::ReadMatchResult(){
  boost::asio::async_read_until(webServerSocket, response, "\r\n",
      [this](boost::system::error_code err, std::size_t length) {
      std::cout << "Reading match result" << std::endl;
      if (!err) {
          std::istream response_stream(&this->response);
          std::string http_version;
          response_stream >> http_version;
          unsigned int status_code;
          response_stream >> status_code; // THIS LINE IS CAUSING SEGMENTTION FAULT -> BUT IT"S RELATED TO STREAMBUF
          std::string status_message;
          std::getline(response_stream, status_message);
          if (!response_stream || http_version.substr(0, 5) != "HTTP/")
          {
            std::cout << "Invalid response\n";
          }
          if (status_code != 201)
          {
            std::cout << "Response returned with status code " << status_code << "\n";
          }

          std::cout << "Posted Result" << std::endl;
      } else {
        std::cout << "Error reading results response" << std::endl;
      }
  });
}

void User::StartPostMap(std::string input){
/*
  using namespace boost::archive::iterators;
  typedef transform_width<binary_from_base64<remove_whitespace
    <std::string::const_iterator> >, 8, 6> ItBinaryT;

    // If the input isn't a multiple of 4, pad with =
  size_t num_pad_chars((4 - input.size() % 4) % 4);
  input.append(num_pad_chars, '=');

  size_t pad_chars(std::count(input.begin(), input.end(), '='));
  std::replace(input.begin(), input.end(), '=', 'A');
  std::string output(ItBinaryT(input.begin()), ItBinaryT(input.end()));
  output.erase(output.end() - pad_chars, output.end());



  ptree root, info;

  info.put("raw_image", "data:text/plain;base64,..." );
  root.put_child("map", info);

  std::ostringstream buf;
  write_json (buf, root, true);
*/
}

void User::FinishPostMap(){


}
