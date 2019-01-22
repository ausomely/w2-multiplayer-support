#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include <set>
#include <string>
#include <numeric>
#include <boost/asio.hpp>
#include <boost/algorithm/string/join.hpp>

#define MAX_BUFFER 1024
#define DEFAULT_PORT 55107 //same default port of game client
using boost::asio::ip::tcp;

class Participant
{
    public:
        virtual ~Participant() {}
        virtual std::string GetName() = 0;
};

typedef std::shared_ptr<Participant> Participant_ptr;

class Lobby
{
    //class to manage the shared state between clients
    //I'd see it being used to managing overall game communication
    //and data management
    private:
        std::set<Participant_ptr> Users;
        std::vector<std::string> UserNames;
    public:
        //constructor, no need to initialize members yet
        Lobby() {}

        //a new client has joined
        void join(Participant_ptr user) {
            Users.insert(user);
            UserNames.push_back(user->GetName());
        }

        void leave(Participant_ptr user) {
            Users.erase(user);
            for (auto iter = UserNames.begin();
                 iter != UserNames.end(); iter++) {
                if (*iter == user->GetName()) { //name found
                    UserNames.erase(iter);
                    break;
                }
            }
            std::cout << "Client " << user->GetName() << " has left the session!" << std::endl;
            PrintNames();
        }

        void PrintNames() {
            std::cout << "Current Connections: " << std::endl;
            for (auto name : UserNames) {
                std::cout << name << std::endl;
            }
            std::cout << std::endl;
        }
        //a client has left

        //print names and place in buffer
        size_t PrepareUsersInfo(char* buff) {
            //print connections from server's perspective
            PrintNames();

            //join the usernames on newline, put in buffer to send current connections
            std::string Tmp = boost::algorithm::join(UserNames, "\n");
            Tmp = "Current Connected Clients:\n" + Tmp + "\n";
            strcpy(buff, Tmp.c_str());

            //return the length of string plus null byte
            return Tmp.length() + 1;
        }
};

class Session : public Participant, public std::enable_shared_from_this<Session>
{
    //Class for managing a single connection with a client
    private:
        tcp::socket socket;
        char data[MAX_BUFFER];
        std::string name; //username associated with session
        Lobby& lobby; //shared lobby object

        std::string GetName() {
            return name;
        }

        void DoRead()
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

        void DoWrite()
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

    public:
        Session(tcp::socket socket_, Lobby& lobby_)
            : socket(std::move(socket_)), lobby(lobby_) {}

        //start reading from connection
        void Start()
        {
            DoRead();
        }
};


class Server
{
    private:
        tcp::acceptor acceptor;
        tcp::socket socket;
        Lobby lobby;

        void DoAccept() {
            acceptor.async_accept(socket,
                [this](boost::system::error_code err) {
                if (!err) {
                    //accept new connection and create new Session for it
                    std::make_shared<Session>(std::move(socket), lobby)->Start();
                }
                //continue to listen for new connections
                DoAccept();
            });
        }
    public:
        Server(boost::asio::io_service& io_service, short port)
            : acceptor(io_service, tcp::endpoint(tcp::v4(), port)), socket(io_service) {
            //listen for new connections
            DoAccept();
        }
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
            std::cout << "No port specified, using 55107 as default" << std::endl;
            Port = DEFAULT_PORT;
        }

        boost::asio::io_service io_service;

        //create serever object
        Server s(io_service, Port);
        std::cout << "Server started!" << std::endl;

        //run io_service loop
        io_service.run();
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
