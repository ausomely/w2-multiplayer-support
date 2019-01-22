#include "User.h"
#include "Session.h"
#include "LoginSession.h"

void User::InitializeSession() {
    CurrentSession = LoginSession::Instance();
    CurrentSession->Start(shared_from_this());
}
