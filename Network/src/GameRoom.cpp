#include "GameRoom.h"
#include "User.h"
#include <boost/bind.hpp>

GameRoom::GameRoom(std::shared_ptr<User> host, const RoomInfo::RoomInformation &roomInformation)
    : capacity(roomInformation.capacity()), size(1), map(roomInformation.map()) {
    owner = host;
    roomInfo.CopyFrom(roomInformation);
    players.insert(owner);
    host->id = 0;
}

void GameRoom::join(std::shared_ptr<User> user) {
    user->id = size;
    size++;
    players.insert(user);
    roomInfo.set_size(size);
}

void GameRoom::leave(std::shared_ptr<User> user) {
    size--;
    roomInfo.set_size(size);
    user->id = -1;
    players.erase(user);
    if (user == owner) {
        owner = *players.begin();
        roomInfo.set_host((*players.begin())->name);
    }
}

void GameRoom::SetPlayerComand(const GameInfo::PlayerCommandRequest &playerCommandRequest, int index) {
    playerCommandPackage.mutable_dplayercommand(index)->CopyFrom(playerCommandRequest);
}

const RoomInfo::RoomInformation& GameRoom::GetRoomInfo() const {
    return roomInfo;
}

void GameRoom::SetData(char* data) {
    size_t size = playerCommandPackage.ByteSizeLong();
    playerCommandPackage.SerializeToArray(data, size);
}

void GameRoom::InitializeGame() {
    for (int i = 0; i < size; i++) {
        playerCommandPackage.add_dplayercommand();
    }
}
