#include "User.h"

void User::InitializeSession() {
    currentSession = LoginSession::Instance();
    currentSession->Start(shared_from_this());
}

void User::ChangeSession(std::shared_ptr<Session> session) {
    currentSession = session;
    currentSession->Start(shared_from_this());
}

// tell the client its done with updating info
void User::SendFinish() {
    std::string message = "Finish";
    boost::asio::async_write(socket, boost::asio::buffer(message.c_str(), MAX_BUFFER),
        [this](boost::system::error_code err, std::size_t ) {
        if (!err) {

        }
    });
}
