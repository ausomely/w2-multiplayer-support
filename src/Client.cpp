#include "Client.h"
#include "LoginInfo.pb.h"

using boost::asio::ip::tcp;

Client::Client(std::string username_, std::string password_)
    : socket(io_service), resolver(io_service), username(username_), password(password_) {

}

bool Client::Connect(std::string hostName, int portNumber){
    boost::system::error_code err;
    boost::asio::connect(socket, resolver.resolve({hostName, std::to_string(portNumber)}), err);
    if(err) {
        std::cerr << "ERROR connecting. Check your hostname and port number." << std::endl;
        return false;
    }

    return true;
}

void Client::SendLoginInfo() {
    boost::system::error_code err;
    LoginInfo::Credential credential;

    credential.set_username(username);
    credential.set_password(password);

    boost::asio::streambuf stream_buffer;
    std::ostream output_stream(&stream_buffer);

    credential.SerializeToOstream(&output_stream);

    boost::asio::write(socket, stream_buffer, err);//boost::asio::buffer(buffer, buffer.size()), err);
    if(err) {
        std::cerr << "ERROR writing" << std::endl;
        return;
    }

    char response[BUFFER_SIZE];
    boost::asio::read(socket, boost::asio::buffer(response, BUFFER_SIZE), err);
    if(err) {
        std::cerr << "ERROR reading" << std::endl;
        return;
    }

    std::cout << response << std::endl;
    return;
}

void Client::CloseConnection(){
    socket.close();
}
