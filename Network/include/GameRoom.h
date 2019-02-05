#ifndef GAMEROOM_H
#define GAMEROOM_H

#include <set>
#include <string>
#include <memory>
#include "GameInfo.pb.h"
#include "RoomInfo.pb.h"

class User;

using namespace GameInfo;

class GameRoom: public std::enable_shared_from_this<GameRoom>
{
    /* class to store the info about the game room
       infomration to keep track of:
           the list of User objects in the room,
           the current number of players,
           maximum number of players,
           the name of the map
    */
    protected:
        std::set< std::shared_ptr<User> > players;
        std::shared_ptr<User> owner;
        int capacity;
        int size;
        std::string map;
        GameInfo::PlayerCommandPackage playerCommandPackage;
        RoomInfo::RoomInformation roomInfo;
    public:
        GameRoom(std::shared_ptr<User> host, const RoomInfo::RoomInformation &roomInformation);
        void join(std::shared_ptr<User> user);
        void leave(std::shared_ptr<User> user);
        void SetPlayerComand(const GameInfo::PlayerCommandRequest &playerCommandRequest, int index);
        const RoomInfo::RoomInformation& GetRoomInfo() const;
        void InitializeGame();
        void SetData(char* data);
};

#endif
