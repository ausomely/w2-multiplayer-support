#include "Client.h"

using boost::asio::ip::tcp;

Client::Client() : socket(io_service), resolver(io_service) {

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

void Client::SendMessage(std::string data){
    boost::system::error_code err;
    boost::asio::write(socket, boost::asio::buffer(data, data.size()), err);
    if(err) {
        std::cerr << "ERROR writing" << std::endl;
        return;
    }

    std::string response;
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
