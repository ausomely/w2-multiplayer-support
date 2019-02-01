#include "User.h"

void User::InitializeSession() {
    currentSession = LoginSession::Instance();
    currentSession->Start(shared_from_this());
}

void User::ChangeSession(std::shared_ptr<Session> session) {
    currentSession = session;
    currentSession->Start(shared_from_this());
}
