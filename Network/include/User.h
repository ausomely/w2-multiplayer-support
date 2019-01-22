#ifndef USER_H
#define USER_H
#include <memory>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
#define MAX_BUFFER 1024

class Session;
class LoginSession;
class Lobby;

class User: public std::enable_shared_from_this<User>
{
    friend class Session;
    friend class LoginSession;
    friend class Lobby;
    protected:
        tcp::socket socket;
        char data[MAX_BUFFER];
        std::string name; //username associated with session
        Lobby& lobby; //shared lobby object
        std::shared_ptr<Session> CurrentSession;
    public:
        User(tcp::socket socket_, Lobby& lobby_)
            : socket(std::move(socket_)), lobby(lobby_) {}
        void InitializeSession();
};

typedef std::shared_ptr<User> User_ptr;

#endif
