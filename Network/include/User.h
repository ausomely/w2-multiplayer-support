#ifndef USER_H
#define USER_H
#include <memory>
#include <boost/asio.hpp>
#include "Session.h"
#include "LoginSession.h"
#include "AcceptedSession.h"
#include "InGameSession.h"
#include "FindGameSession.h"

class Lobby;

using boost::asio::ip::tcp;
#define MAX_BUFFER 1024

class User: public std::enable_shared_from_this<User>
{
    friend class Session;
    friend class LoginSession;
    friend class AcceptedSession;
    friend class InGameSession;
    friend class FindGameSession;
    friend class Lobby;
    protected:
        tcp::socket socket;
        char data[MAX_BUFFER];
        std::string password;
        std::string name; //username associated with session
        Lobby& lobby; //shared lobby object
        std::shared_ptr<Session> CurrentSession;
    public:
        User(tcp::socket socket_, Lobby& lobby_)
            : socket(std::move(socket_)), lobby(lobby_) {}
        void InitializeSession();
        void ChangeSession(std::shared_ptr<Session> session);
};

#endif
