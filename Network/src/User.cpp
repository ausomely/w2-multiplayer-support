#include "User.h"
#include "Session.h"
#include "LoginSession.h"

void User::InitializeSession() {
    CurrentSession = std::make_shared<LoginSession>();
    CurrentSession->Start(shared_from_this());
}
