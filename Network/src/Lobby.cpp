#include "Lobby.h"

//a new client has joined
void Lobby::join(Session_ptr user) {
    Users.insert(user);
    UserNames.push_back(user->GetName());
}

//a client has left
void Lobby::leave(Session_ptr user) {
    Users.erase(user);
    for (auto iter = UserNames.begin();
         iter != UserNames.end(); iter++) {
        if (*iter == user->GetName()) { //name found
            UserNames.erase(iter);
            break;
        }
    }
    std::cout << "Client " << user->GetName() << " has left the session!" << std::endl;
    PrintNames();
}

void Lobby::PrintNames() {
    std::cout << "Current Connections: " << std::endl;
    for (auto name : UserNames) {
        std::cout << name << std::endl;
    }
    std::cout << std::endl;
}

//print names and place in buffer
size_t Lobby::PrepareUsersInfo(char* buff) {
    //print connections from server's perspective
    PrintNames();

    //join the usernames on newline, put in buffer to send current connections
    std::string Tmp = boost::algorithm::join(UserNames, "\n");
    Tmp = "You are connected!\nCurrent Connected Clients:\n" + Tmp + "\n";
    strcpy(buff, Tmp.c_str());

    //return the length of string plus null byte
    return Tmp.length() + 1;
}
