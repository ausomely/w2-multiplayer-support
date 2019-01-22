#ifndef LOBBY_H
#define LOBBY_H

#include <boost/algorithm/string/join.hpp>
#include <set>
#include "Session.h"
#include "User.h"

class Lobby
{
    //class to manage the shared state between clients
    //I'd see it being used to managing overall game communication
    //and data management
    private:
        std::set<User_ptr> Users;
        std::vector<std::string> UserNames;
    public:
        //constructor, no need to initialize members yet
        Lobby() {}

        //a new client has joined
        void join(User_ptr UserPtr);

        //a client has left
        void leave(User_ptr UserPtr);

        //print the current connected clients
        void PrintNames();

        //print names and place in buffer
        size_t PrepareUsersInfo(char* buff);
};

#endif
