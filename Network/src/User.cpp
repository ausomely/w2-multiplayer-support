#include "User.h"

void User::InitializeSession() {
    currentSession = LoginSession::Instance();
    currentSession->Start(shared_from_this());
}

void User::ChangeSession(std::shared_ptr<Session> session) {
    currentSession = session;
    currentSession->Start(shared_from_this());
}

void User::Deliver(const GameInfo::PlayerCommandRequest &playerCommandRequest) {
    bool writeInProgress = !playerCommands.empty();
    playerCommands.push_back(playerCommandRequest);
    if(!writeInProgress) {
        currentSession->DoWrite(shared_from_this());
    }
}
