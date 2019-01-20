#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include <string>
#include <numeric>
#include <boost/asio.hpp>
#include <boost/algorithm/string/join.hpp>

#define MAX_BUFFER 1024
#define DEFAULT_PORT 55107 //same default port of game client
using boost::asio::ip::tcp;

class Lobby
{
//class to manage the shared state between clients
//I'd see it being used to managing overall game communication
//and data management

public:
  //constructor, no need to initialize members yet
  Lobby() {}
  
  //print names and place in buffer
  size_t PrintNames(char* buff)
  {
    //print connections from server's perspective
    for (auto Name : UserNames) {
      std::cout << Name << std::endl;
    }

    //join the usernames on newline, put in buffer to send current connections
    std::string Tmp = boost::algorithm::join(UserNames, "\n");
    strcpy(buff, Tmp.c_str());

    //return the length of string plus null byte
    return Tmp.length() + 1;
  }

  std::vector<std::string> UserNames; //list of usernames connected to server
};

class Session
  : public std::enable_shared_from_this<Session>
{
//Class for managing a single connection with a client

public:
  Session(tcp::socket socket, Lobby& clients)
    : Socket(std::move(socket)), Clients(clients)
  {
  }
  
  //start reading from connection
  void Start()
  {
    DoRead();
  }

private:
  void DoRead()
  {
    //currently assume there is only one message being sent to server
    //and that message being the username
    auto self(shared_from_this());
    bzero(Data, MAX_BUFFER);
    Socket.async_read_some(boost::asio::buffer(Data, MAX_BUFFER),
        [this, self](boost::system::error_code err, std::size_t length) {
          //data to be processed
          if (!err) {
            //set session's username
            this->Name = std::string(Data);
            //add name to list of users
            this->Clients.UserNames.push_back(std::string(Data));
            DoWrite();
          }
          
          //end of connection
          else if ((boost::asio::error::eof == err) ||
                    (boost::asio::error::connection_reset == err)) {
            //find username in Lobby clients and remove data
            for (auto iter = this->Clients.UserNames.begin(); 
                 iter != this->Clients.UserNames.end(); iter++) {
              if (*iter == this->Name) { //name found
                this->Clients.UserNames.erase(iter);
                break;
              }
            }
          }
        });
  }

  void DoWrite()
  {
    auto self(shared_from_this());
    //print names of current connections and put in buffer
    size_t Length = Clients.PrintNames(Data);
    
    //write list of clients to socket
    boost::asio::async_write(Socket, boost::asio::buffer(Data, Length),
        [this, self](boost::system::error_code err, std::size_t ) {
          //if no error, continue trying to read from socket
          if (!err) {
            DoRead();
          }
        });
  }

  tcp::socket Socket;
  char Data[MAX_BUFFER];
  std::string Name; //username associated with session
  Lobby& Clients; //shared lobby object
};


class Server
{
public:
  Server(boost::asio::io_service& io_service, short port)
    : Acceptor(io_service, tcp::endpoint(tcp::v4(), port)),
      Socket(io_service) {
    //listen for new connections
    DoAccept();
  }

private:
  void DoAccept() {
    Acceptor.async_accept(Socket,
        [this](boost::system::error_code err) {
          if (!err) {
            //accept new connection and create new Session for it
            std::make_shared<Session>(std::move(Socket), Clients)->Start();
          }
          //continue to listen for new connections
          DoAccept();
        });
  }

  tcp::acceptor Acceptor;
  tcp::socket Socket;
  Lobby Clients;
};

int main(int argc, char* argv[])
{
  try {
    int Port;

    if (argc > 2) { //more arguments than expected
      std::cerr << "Usage: async_tcp_echo_server [port]\n";
      return 1;
    }
    else if (argc == 2) { //port provided
      Port = std::atoi(argv[1]);
    }
    else { //no port specified
      Port = DEFAULT_PORT;
    }

    boost::asio::io_service io_service;

    //create serever object
    Server s(io_service, Port); 
    
    //run io_service loop
    io_service.run();
  }
  catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
