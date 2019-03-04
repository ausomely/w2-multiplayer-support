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
    endNum = 0;
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
        if(It->currentSession == InRoomSession::Instance()) {
            boost::asio::async_write(It->socket, stream_buffer,
                [It](boost::system::error_code err, std::size_t ) {

                if (!err) {
                }
            });
        }
    }
}

void GameRoom::SendRoomInfo(std::shared_ptr<User> user) {
    boost::asio::streambuf stream_buffer;
    std::ostream output_stream(&stream_buffer);
    roomInfo.SerializeToOstream(&output_stream);
    boost::asio::async_write(user->socket, stream_buffer,
        [user](boost::system::error_code err, std::size_t ) {

        if (!err) {
        }
    });
}

void GameRoom::UpdateRoomList(std::shared_ptr<User> user) {
    // update room list
    boost::asio::streambuf stream_buffer;
    std::ostream output_stream(&stream_buffer);
    RoomInfo::RoomInfoPackage roomList = user->lobby.GetRoomList();
    roomList.SerializeToOstream(&output_stream);
    for(auto &It: user->lobby.users) {
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
    roomInfo.set_active(true);
    for(int i = 0; i < size; i++) {
        GameInfo::PlayerCommandRequest playerCommandRequest;
        playerCommandRequest.set_playernum(i + 1);
        playerCommandRequest.set_daction(0);
        playerCommandRequest.set_dtargetnumber(0);
        playerCommandRequest.set_dtargettype(0);

        GameInfo::PlayerCommandRequest::CPixelPosition* pixelPosition = new GameInfo::PlayerCommandRequest::CPixelPosition();
        pixelPosition->set_dx(0);
        pixelPosition->set_dy(0);

        playerCommandRequest.set_allocated_dtargetlocation(pixelPosition);
        playerCommandPackage.add_dplayercommand()->CopyFrom(playerCommandRequest);
    }
    // set tcp_no_delay for sending game play data
    for(auto& It: players) {
        boost::asio::ip::tcp::no_delay option(true);
        It->socket.set_option(option);
    }
}

void GameRoom::IncreaseEndNum() {
    endNum++;
    if(endNum == size) {
      EndGame();
    }
}

void GameRoom::EndGame() {
    roomInfo.set_active(false);
    playerCommandPackage.Clear();
    for(int i = 2; i <= size; i++) {
        roomInfo.set_ready(i, false);
    }
    for(auto& It: players) {
        // set tcp_no_delay false
        boost::asio::ip::tcp::no_delay option(false);
        It->socket.set_option(option);
    }
}
