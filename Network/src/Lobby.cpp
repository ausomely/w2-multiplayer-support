#include "Lobby.h"

//a new client has joined
void Lobby::join(std::shared_ptr<User>  userPtr) {
    users.insert(userPtr);
    userNames.push_back(userPtr->name);
}

//a client has left
void Lobby::leave(std::shared_ptr<User>  userPtr) {
    users.erase(userPtr);
    for (auto iter = userNames.begin();
         iter != userNames.end(); iter++) {
        if (*iter == userPtr->name) { //name found
            userNames.erase(iter);
            break;
        }
    }
    std::cout << "Client " << userPtr->name << " has left the session!" << std::endl;
    PrintNames();
}

void Lobby::PrintNames() {
    std::cout << "Current Connections: " << std::endl;
    for (auto name : userNames) {
        std::cout << name << std::endl;
    }
    std::cout << std::endl;
}

//print names and place in buffer
size_t Lobby::PrepareUsersInfo(char* buff) {
    //print connections from server's perspective
    PrintNames();

    //join the usernames on newline, put in buffer to send current connections
    std::string Tmp = boost::algorithm::join(userNames, "\n");
    Tmp = "You are connected!\nCurrent Connected Clients:\n" + Tmp + "\n";
    strcpy(buff, Tmp.c_str());

    //return the length of string plus null byte
    return Tmp.length() + 1;
}

//add a game room to the list
void Lobby::AddRoom(std::shared_ptr<GameRoom> room) {
    gameRooms.push_back(room);
}

//remove a game room from the list
void Lobby::RemoveRoom(std::shared_ptr<GameRoom> room) {
    for (auto It = gameRooms.begin(); It != gameRooms.end();) {
        if(*It == room) {
            gameRooms.erase(It);
        }
    }
}

//get a protobuf package list of rooms info from the set of gameRooms
RoomInfo::RoomInfoPackage Lobby::GetRoomList() {
    RoomInfo::RoomInfoPackage roomList;
    for (auto& room: gameRooms) {
        roomList.add_roominfo()->CopyFrom(room->GetRoomInfo());
    }

    return roomList;
}

//join a user to specific room
void Lobby::JoinRoom(std::shared_ptr<User> userPtr, int index) {
    gameRooms[index]->join(userPtr);
    userPtr->currentRoom = gameRooms[index];
}
