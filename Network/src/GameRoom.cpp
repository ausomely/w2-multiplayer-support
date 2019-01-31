#include "GameRoom.h"
#include "User.h"
#include <boost/bind.hpp>

GameRoom::GameRoom(std::shared_ptr<User> host, int maximumPlayers, std::string mapName)
    : capacity(maximumPlayers), size(1), map(mapName) {
    owner = host;
    players.insert(owner);
}
void GameRoom::Deliver(const GameInfo::PlayerCommandRequest &playerCommandRequest) {
    std::for_each(players.begin(), players.end(),
        boost::bind(&User::Deliver, _1, boost::ref(playerCommandRequest)));
}

void GameRoom::join(std::shared_ptr<User> user) {
    size++;
    players.insert(user);
}
void GameRoom::leave(std::shared_ptr<User> user) {
    size--;
    players.erase(user);
    if (user == owner) {
        owner = *players.begin();
    }

}
