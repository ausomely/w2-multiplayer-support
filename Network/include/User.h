#ifndef USER_H
#define USER_H
#include <memory>
#include <boost/asio.hpp>
#include <deque>
#include "Session.h"
#include "LoginSession.h"
#include "AcceptedSession.h"
#include "InGameSession.h"
#include "FindGameSession.h"
#include "HostGameSession.h"
#include "InRoomSession.h"
#include "GameRoom.h"

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
    friend class HostGameSession;
    friend class InRoomSession;
    friend class Lobby;
    friend class GameRoom;
    protected:
        boost::asio::io_service& io_service;
        tcp::socket socket;
        tcp::socket webServerSocket;
        char data[MAX_BUFFER];
        std::string password;
        std::string name; //username associated with session
        std::string jwt;
        Lobby& lobby; //shared lobby object
        std::weak_ptr<GameRoom> currentRoom;
        boost::asio::streambuf response;
        std::shared_ptr<Session> currentSession;
        int id; // -1 if not in a room
        int playerNum;
    public:
        User(tcp::socket socket_, Lobby& lobby_, boost::asio::io_service& io_serv);
        void InitializeSession();
        void ChangeSession(std::shared_ptr<Session> session);
        void SendFinish();
        void WriteMatchResult(bool win);
        void ReadMatchResult();
        void StartPostMap(std::string input);
        void FinishPostMap();
        void Logout();
        void ConnectToServer();

};

#endif
