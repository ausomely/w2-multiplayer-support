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
    tcp::resolver::iterator endpoint_iterator;
    endpoint_iterator = resolver.resolve({context->DRemoteHostname, std::to_string(context->DMultiplayerPort)}, err);
    if(err) {
        std::cerr << "ERROR connecting. Check your hostname and port number." << std::endl;
        return false;
    }
    boost::asio::connect(socket, endpoint_iterator, err);
    if(err) {
        std::cerr << "ERROR connecting. Check your hostname and port number." << std::endl;
        return false;
    }

    return true;
}

// send log in information: username, password
bool Client::SendLoginInfo(std::shared_ptr<CApplicationData> context) {
    boost::system::error_code err;
    LoginInfo::Credential credential;

    credential.set_username(context->DUsername);
    credential.set_password(context->DPassword);

    boost::asio::streambuf stream_buffer;
    std::ostream output_stream(&stream_buffer);

    credential.SerializeToOstream(&output_stream);

    boost::asio::write(socket, stream_buffer, err);
    if(err) {
        std::cerr << "ERROR writing" << std::endl;
        return false;
    }

    char response[BUFFER_SIZE];
    boost::asio::read(socket, boost::asio::buffer(response, BUFFER_SIZE), err);
    if(err) {
        std::cerr << "ERROR reading" << std::endl;
        return false;
    }

    if(strcmp(response, "Your login information is wrong. Please try again\n") == 0) {
        std::cout << response << std::endl;
        return false;
    }
    std::cout << response << std::endl;
    return true;
}

// send packages of game info: SPlayerCommandRequest
void Client::SendGameInfo(std::shared_ptr<CApplicationData> context) {
    // initialize package
    GameInfo::PlayerCommandRequest playerCommandRequest;

    int DPlayerNumber = to_underlying(context->DPlayerNumber);

    playerCommandRequest.set_playernum(DPlayerNumber);
    playerCommandRequest.set_daction(to_underlying(context->DPlayerCommands[DPlayerNumber].DAction));
    playerCommandRequest.set_dtargetnumber(to_underlying(context->DPlayerCommands[DPlayerNumber].DTargetNumber));
    playerCommandRequest.set_dtargettype(to_underlying(context->DPlayerCommands[DPlayerNumber].DTargetType));

    for (auto &It: context->DPlayerCommands[DPlayerNumber].DActors) {
        playerCommandRequest.add_dactors(It.lock()->Id());
    }

    GameInfo::PlayerCommandRequest::CPixelPosition* pixelPosition = new GameInfo::PlayerCommandRequest::CPixelPosition();
    pixelPosition->set_dx(context->DPlayerCommands[DPlayerNumber].DTargetLocation.X());
    pixelPosition->set_dy(context->DPlayerCommands[DPlayerNumber].DTargetLocation.Y());

    playerCommandRequest.set_allocated_dtargetlocation(pixelPosition);

    // write package to ostream: a file
    std::ofstream outfile;

    outfile.open("LocalStreamCommand.bin", std::ios_base::app | std::ios::binary);

    playerCommandRequest.SerializeToOstream(&outfile);
    //outfile << playerCommandRequest.DebugString();

    outfile.close();

    // send package to server
    boost::system::error_code err;
    boost::asio::streambuf stream_buffer;
    std::ostream output_stream(&stream_buffer);
    playerCommandRequest.SerializeToOstream(&output_stream);

    boost::asio::write(socket, stream_buffer, err);
    if(err) {
        std::cerr << "ERROR writing" << std::endl;
        return;
    }

    return;
}

// send the server the information of the room hosted
void Client::SendRoomInfo(std::shared_ptr<CApplicationData> context) {

    RoomInfo::RoomInformation roomInfo;
    roomInfo.set_host(context->DUsername);

    //std::string mapName =
    roomInfo.set_map(context->DSelectedMap->MapName());
    roomInfo.set_size(1);
    roomInfo.set_capacity(context->DSelectedMap->PlayerCount());

    // send package to server
    boost::system::error_code err;
    boost::asio::streambuf stream_buffer;
    std::ostream output_stream(&stream_buffer);
    roomInfo.SerializeToOstream(&output_stream);

    boost::asio::write(socket, stream_buffer, err);
    if(err) {
        std::cerr << "ERROR writing" << std::endl;
        return;
    }

    return;
}

// get the room list information from client
RoomInfo::RoomInfoPackage Client::GetRoomList(std::shared_ptr<CApplicationData> context) {
    RoomInfo::RoomInfoPackage roomList;
    boost::system::error_code err;

    char data[BUFFER_SIZE];
    std::size_t length = boost::asio::read(socket, boost::asio::buffer(data, BUFFER_SIZE), err);
    if(err) {
        std::cerr << "ERROR reading" << std::endl;
        return roomList;
    }
    roomList.ParseFromArray(data, length);

    //std::cout << roomList.DebugString() << std::endl;
    return roomList;
}

// send the server a message
void Client::SendMessage(std::string message) {
    boost::system::error_code err;

    boost::asio::write(socket, boost::asio::buffer(message.c_str(), strlen(message.c_str())), err);
    if(err) {
        std::cerr << "ERROR writing" << std::endl;
        return;
    }
    return;
}

void Client::GetGameInfo(std::shared_ptr<CApplicationData> context){
    char data[BUFFER_SIZE];
    bzero(data, BUFFER_SIZE);
    boost::system::error_code err;
    size_t length =  socket.read_some(boost::asio::buffer(data, BUFFER_SIZE), err);
    GameInfo::PlayerCommandRequest playerCommandRequest;
    playerCommandRequest.ParseFromArray(data,length);
    std::cerr << playerCommandRequest.DebugString() << std::endl;

        //context->
      /*  context->DPlayerCommands[playerCommandRequest.playernum()].DAction = (EAssetCapabilityType)(playerCommandRequest.daction());
        context->DPlayerCommands[playerCommandRequest.playernum()].DTargetNumber = (EPlayerNumber)(playerCommandRequest.dtargetnumber());
        context->DPlayerCommands[playerCommandRequest.playernum()].DTargetType = (EAssetType)(playerCommandRequest.dtargettype());
        context->DPlayerCommands[playerCommandRequest.playernum()].DTargetLocation.SetXFromTile(playerCommandRequest.mutable_dtargetlocation()->dx());
        context->DPlayerCommands[playerCommandRequest.playernum()].DTargetLocation.SetYFromTile(playerCommandRequest.mutable_dtargetlocation()->dy());
      */
}

// Close the conenction fromm server
void Client::CloseConnection(){
    socket.close();
}
