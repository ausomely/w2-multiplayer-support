#include "Client.h"
#include "ApplicationData.h"
#include "LoginInfo.pb.h"
#include "GameInfo.pb.h"
#include <fstream>

using boost::asio::ip::tcp;

// create a client
Client::Client()
    : socket(io_service), resolver(io_service) {

}

// connect the client to the server
bool Client::Connect(std::shared_ptr<CApplicationData> context){
    boost::system::error_code err;
    boost::asio::connect(socket, resolver.resolve({context->DRemoteHostname, std::to_string(context->DMultiplayerPort)}), err);
    if(err) {
        std::cerr << "ERROR connecting. Check your hostname and port number." << std::endl;
        return false;
    }

    return true;
}

// send log in information: username, password
void Client::SendLoginInfo(std::shared_ptr<CApplicationData> context) {
    boost::system::error_code err;
    LoginInfo::Credential credential;

    credential.set_username(context->DUsername);
    credential.set_password(context->DPassword);

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

// send packages of game info: SPlayerCommandRequest
void Client::SendGameInfo(std::shared_ptr<CApplicationData> context) {
    // initialize package
    GameInfo::PlayerCommandRequest playerCommandRequset;

    int DPlayerNumber = to_underlying(context->DPlayerNumber);

    playerCommandRequset.set_daction(to_underlying(context->DPlayerCommands[DPlayerNumber].DAction));
    playerCommandRequset.set_dtargetnumber(to_underlying(context->DPlayerCommands[DPlayerNumber].DTargetNumber));
    playerCommandRequset.set_dtargettype(to_underlying(context->DPlayerCommands[DPlayerNumber].DTargetType));

    for (auto &It: context->DPlayerCommands[DPlayerNumber].DActors) {
        playerCommandRequset.add_dactors(It.lock()->Id());
    }

    GameInfo::PlayerCommandRequest::CPixelPosition* pixelPosition = new GameInfo::PlayerCommandRequest::CPixelPosition();
    pixelPosition->set_dx(context->DPlayerCommands[DPlayerNumber].DTargetLocation.X());
    pixelPosition->set_dy(context->DPlayerCommands[DPlayerNumber].DTargetLocation.Y());

    playerCommandRequset.set_allocated_dtargetlocation(pixelPosition);

    // write package to ostream: a file
    std::ofstream outfile;

    outfile.open("LocalStreamCommand.bin", std::ios_base::app);

    playerCommandRequset.SerializeToOstream(&outfile);

    outfile.close();

    // send package to server
    boost::system::error_code err;
    boost::asio::streambuf stream_buffer;
    std::ostream output_stream(&stream_buffer);
    playerCommandRequset.SerializeToOstream(&output_stream);

    boost::asio::write(socket, stream_buffer, err);//boost::asio::buffer(buffer, buffer.size()), err);
    if(err) {
        std::cerr << "ERROR writing" << std::endl;
        return;
    }

    return;
}


// Close the conenction fromm server
void Client::CloseConnection(){
    socket.close();
}
