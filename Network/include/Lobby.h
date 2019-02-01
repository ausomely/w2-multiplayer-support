#ifndef LOBBY_H
#define LOBBY_H

#include <boost/algorithm/string/join.hpp>
#include <set>
#include <vector>
#include "User.h"
#include "RoomInfo.pb.h"

class Lobby
{
    /* class to manage the shared state between clients
       Used to managing overall game communication and data management
    */
    private:
        std::set<std::shared_ptr<User> > users;
        std::vector<std::string> userNames;
        std::set<std::shared_ptr<GameRoom>> gameRooms;

    public:
        //constructor, no need to initialize members yet
        Lobby() {}

        //a new client has joined
        void join(std::shared_ptr<User>  userPtr);

        //a client has left
        void leave(std::shared_ptr<User>  userPtr);

        //print the current connected clients
        void PrintNames();

        //print names and place in buffer
        size_t PrepareUsersInfo(char* buff);

        //add a game room
        void AddRoom(std::shared_ptr<GameRoom> room);

        //remove a game room
        void RemoveRoom(std::shared_ptr<GameRoom> room);

        //convert to a protobuf package for all the rooms
        RoomInfo::RoomInfoPackage GetRoomList();

};

#endif
