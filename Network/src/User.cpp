#include "User.h"

void User::InitializeSession() {
    CurrentSession = LoginSession::Instance();
    CurrentSession->Start(shared_from_this());
}

void User::ChangeSession(std::shared_ptr<Session> session) {
    CurrentSession = session;
    CurrentSession->Start(shared_from_this());
}
