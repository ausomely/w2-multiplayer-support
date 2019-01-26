#ifndef LOBBY_H
#define LOBBY_H

#include <boost/algorithm/string/join.hpp>
#include <set>
#include <vector>

class User;

class Lobby
{
    //class to manage the shared state between clients
    //I'd see it being used to managing overall game communication
    //and data management
    private:
        std::set<std::shared_ptr<User> > Users;
        std::vector<std::string> UserNames;
    public:
        //constructor, no need to initialize members yet
        Lobby() {}

        //a new client has joined
        void join(std::shared_ptr<User>  UserPtr);

        //a client has left
        void leave(std::shared_ptr<User>  UserPtr);

        //print the current connected clients
        void PrintNames();

        //print names and place in buffer
        size_t PrepareUsersInfo(char* buff);
};

#endif
