#ifndef LOBBY_H
#define LOBBY_H

#include "Session.h"
#include <boost/algorithm/string/join.hpp>
#include <set>

class Lobby
{
    //class to manage the shared state between clients
    //I'd see it being used to managing overall game communication
    //and data management
    private:
        std::set<Session_ptr> Users;
        std::vector<std::string> UserNames;
    public:
        //constructor, no need to initialize members yet
        Lobby() {}

        //a new client has joined
        void join(Session_ptr user);

        //a client has left
        void leave(Session_ptr user);

        //print the current connected clients
        void PrintNames();

        //print names and place in buffer
        size_t PrepareUsersInfo(char* buff);
};

#endif
