#include "GameRoom.h"
#include "User.h"
#include <boost/bind.hpp>

GameRoom::GameRoom(std::shared_ptr<User> host, int maximumPlayers, std::string mapName)
    : capacity(maximumPlayers), size(1), map(mapName) {
    owner = host;
    players.insert(owner);
    for (int i = 0; i < capacity; i++) {
        playerCommandPackage.add_dplayercommand();
    }
}

void GameRoom::join(std::shared_ptr<User> user) {
    user->id = size;
    size++;
    players.insert(user);
}

void GameRoom::leave(std::shared_ptr<User> user) {
    size--;
    user->id = -1;
    players.erase(user);
    if (user == owner) {
        owner = *players.begin();
    }
}

void GameRoom::SetPlayerComand(const GameInfo::PlayerCommandRequest &playerCommandRequest, int index) {
    playerCommandPackage.mutable_dplayercommand(index)->CopyFrom(playerCommandRequest);
}

void GameRoom::SetData(char* data) {
    size_t size = playerCommandPackage.ByteSizeLong();
    playerCommandPackage.SerializeToArray(data, size);
}
