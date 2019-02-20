#include "GameRoom.h"
#include "User.h"
#include <boost/bind.hpp>
#include "Lobby.h"

GameRoom::GameRoom(std::shared_ptr<User> host, const RoomInfo::RoomInformation &roomInformation)
    : capacity(roomInformation.capacity()), size(1), map(roomInformation.map()) {
    owner = host;
    roomInfo.CopyFrom(roomInformation);
    players.push_back(owner);
    host->id = 1;
}

void GameRoom::CopyRoomInfo(const RoomInfo::RoomInformation &roomInformation) {
    roomInfo.CopyFrom(roomInformation);
}

void GameRoom::join(std::shared_ptr<User> user) {
    size++;
    user->id = size;
    players.push_back(user);
    roomInfo.set_players(size, user->name);
    roomInfo.set_types(size, 1);
    roomInfo.set_ready(size, false);
    roomInfo.set_size(size);

    UpdateRoomInfo();
}

void GameRoom::leave(std::shared_ptr<User> user) {
    size--;
    roomInfo.set_size(size);
    players.erase(std::remove(players.begin(), players.end(), user), players.end());

    if (size == 0) {
        user->lobby.RemoveRoom(user->currentRoom.lock());
        if(user->lobby.gameRooms.size() == 0) {
            for(auto &It : user->lobby.users) {
               // sending notification to those in find game session to clear room list
               if(It->currentSession == FindGameSession::Instance()) {
                   boost::asio::async_write(It->socket, boost::asio::buffer("Empty", MAX_BUFFER),
                       [It](boost::system::error_code err, std::size_t ) {

                       if (!err) {

                       }
                   });
               }
            }
        }
        else {
            UpdateRoomList(user);
        }
    }
    else if (user == owner) {
        owner = *players.begin();
        roomInfo.set_host((*players.begin())->name);
        OrganizeRoomInfo(user->id);
        UpdateRoomList(user);
    }

    else {
        OrganizeRoomInfo(user->id);
        UpdateRoomList(user);
    }

    // marked the user as left
    user->id = -1;
}

// organize the room info when someone left
void GameRoom::OrganizeRoomInfo(int index) {

    // move the players up to fill the spot
    for(int i = index; i <= size; i++) {
        roomInfo.set_players(i, roomInfo.players()[i + 1]);
        roomInfo.set_ready(i, roomInfo.ready()[i + 1]);
        roomInfo.set_types(i, roomInfo.types()[i + 1]);
        players[i - 1]->id--;
    }

    // set the last one info
    roomInfo.set_players(size + 1, "None");
    roomInfo.set_ready(size + 1, false);
    roomInfo.set_types(size + 1, 1);

    UpdateRoomInfo();
}

void GameRoom::BroadcastStartGame() {
    std::string message = "StartGame";
    for(auto &It: players) {
        boost::asio::async_write(It->socket, boost::asio::buffer(message.c_str(), MAX_BUFFER),
            [](boost::system::error_code err, std::size_t ) {
            if (!err) {

            }
        });
    }
}

void GameRoom::UpdateRoomInfo() {
    // update room info for everyone in the game room
    boost::asio::streambuf stream_buffer;
    std::ostream output_stream(&stream_buffer);
    roomInfo.SerializeToOstream(&output_stream);

    for(auto &It: players) {
        // sending notification to other players in the room to update room info
        boost::asio::async_write(It->socket, stream_buffer,
            [It](boost::system::error_code err, std::size_t ) {

            if (!err) {
            }
        });
    }
}

void GameRoom::UpdateRoomList(std::shared_ptr<User> user) {
    // update room list
    boost::asio::streambuf stream_buffer;
    std::ostream output_stream(&stream_buffer);
    RoomInfo::RoomInfoPackage roomList = user->lobby.GetRoomList();
    roomList.SerializeToOstream(&output_stream);
    for(auto &It : user->lobby.users) {
       // sending notification to those in find game session to update room list
       if(It->currentSession == FindGameSession::Instance()) {
           boost::asio::async_write(It->socket, stream_buffer,
               [It](boost::system::error_code err, std::size_t ) {

               if (!err) {

               }
           });
       }
    }
}

void GameRoom::SetPlayerComand(const GameInfo::PlayerCommandRequest &playerCommandRequest, int index) {
    playerCommandPackage.mutable_dplayercommand(index)->CopyFrom(playerCommandRequest);
}

const RoomInfo::RoomInformation& GameRoom::GetRoomInfo() const {
    return roomInfo;
}

const GameInfo::PlayerCommandPackage GameRoom::GetPlayerCommandPackage() const {
    return playerCommandPackage;
}

void GameRoom::InitializeGame() {
    for (int i = 0; i < size; i++) {
        playerCommandPackage.add_dplayercommand();
    }
}
