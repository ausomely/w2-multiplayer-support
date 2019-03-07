#include "Client.h"
#include "ApplicationData.h"
#include "LoginInfo.pb.h"
#include "GameInfo.pb.h"
#include <fstream>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

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

    if(!context->NewMessage.empty()) {
        playerCommandRequest.set_message(context->NewMessage);
        context->NewMessage.clear();
    }

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
    // send package to server
    boost::system::error_code err;
    boost::asio::streambuf stream_buffer;
    std::ostream output_stream(&stream_buffer);
    context->roomInfo.SerializeToOstream(&output_stream);

    boost::asio::write(socket, stream_buffer, err);
    if(err) {
        std::cerr << "ERROR writing" << std::endl;
        return;
    }

    return;
}

void Client::GetRoomInfo(std::shared_ptr<CApplicationData> context) {
    bzero(data, BUFFER_SIZE);
    boost::system::error_code err;
    size_t length = socket.read_some(boost::asio::buffer(data, BUFFER_SIZE), err);
    if(!err) {
        context->roomInfo.ParseFromArray(data, length);
        // update messages
        context->Text.clear();
        for(int Index = 0; Index < 3; Index++) {
            context->Text.push_back(context->roomInfo.messages()[Index]);
        }
        // update DPlayerNumber if someone before you left
        while(context->roomInfo.players()[to_underlying(context->DPlayerNumber)] != context->DUsername) {
            context->DPlayerNumber = static_cast<EPlayerNumber> (to_underlying(context->DPlayerNumber) - 1);
        }
        // copy over room info
        for(int Index = 0; Index < to_underlying(EPlayerColor::Max); Index++) {
            context->DLoadingPlayerTypes[Index] = static_cast <CApplicationData::EPlayerType> (context->roomInfo.types(Index));
            context->DPlayerNames[Index] = context->roomInfo.players(Index);
            context->DReadyPlayers[Index] = context->roomInfo.ready(Index);
            context->DLoadingPlayerColors[Index] = static_cast <EPlayerColor> (context->roomInfo.colors(Index));
        }
    }
    else {
        std::cout << "Error Reading" << std::endl;
    }
}

void Client::UpdateRoomList(std::shared_ptr<CApplicationData> context) {
    bzero(data, BUFFER_SIZE);
    socket.async_read_some(boost::asio::buffer(data, BUFFER_SIZE),
        [this, context](boost::system::error_code err, std::size_t length) {
        if(!err) {

            // empty room list
            if(strcmp(data, "Empty") == 0) {
                context->roomList.Clear();
                UpdateRoomList(context);
            }

            else if(!(strcmp(data, "Finish") == 0)) {
                context->roomList.ParseFromArray(data, length);
                UpdateRoomList(context);
            }
            else {
                std::cout << "Update room list Finish" << std::endl;
            }
        }

        else {
           std::cerr << "ERROR reading" << std::endl;
        }
    });
}

void Client::UpdateRoomInfo(std::shared_ptr<CApplicationData> context) {
    bzero(data, BUFFER_SIZE);
    socket.async_read_some(boost::asio::buffer(data, BUFFER_SIZE),
        [this, context](boost::system::error_code err, std::size_t length) {
        if(!err) {
            // Set flag for a indication of starting game
            if(strcmp(data, "StartGame") == 0) {
                std::string message = "Play";
                boost::system::error_code err;
                boost::asio::write(socket, boost::asio::buffer(message.c_str(), BUFFER_SIZE), err);
                if(!err) {
                    context->roomInfo.set_active(true);
                }
            }
            else if(!(strcmp(data, "Finish") == 0)) {
                context->roomInfo.ParseFromArray(data, length);
                std::cout << context->roomInfo.DebugString() << std::endl;

                // update DPlayerNumber if someone before you left
                while(context->roomInfo.players()[to_underlying(context->DPlayerNumber)] != context->DUsername) {
                    context->DPlayerNumber = static_cast<EPlayerNumber> (to_underlying(context->DPlayerNumber) - 1);
                }
                // update room message
                context->Text.clear();
                for(int Index = 0; Index < 3; Index++) {
                    context->Text.push_back(context->roomInfo.messages()[Index]);
                }

                // copy over room info
                for(int Index = 0; Index < to_underlying(EPlayerColor::Max); Index++) {
                    context->DLoadingPlayerTypes[Index] = static_cast <CApplicationData::EPlayerType> (context->roomInfo.types(Index));
                    context->DPlayerNames[Index] = context->roomInfo.players(Index);
                    context->DReadyPlayers[Index] = context->roomInfo.ready(Index);
                    context->DLoadingPlayerColors[Index] = static_cast <EPlayerColor> (context->roomInfo.colors(Index));
                }
                UpdateRoomInfo(context);
            }
            else {
                std::cout << "Update roominfo Finish" << std::endl;
            }
        }
        else {
           std::cerr << "ERROR reading" << std::endl;
        }
    });
}

// send the server a message
void Client::SendMessage(std::string message) {
    boost::system::error_code err;
    boost::asio::write(socket, boost::asio::buffer(message.c_str(), BUFFER_SIZE), err);
    if(err) {
        std::cerr << "ERROR writing" << std::endl;
        return;
    }
    return;
}

void Client::GetGameInfo(std::shared_ptr<CApplicationData> context) {
    bzero(data, BUFFER_SIZE);
    boost::system::error_code err;
    size_t length = socket.read_some(boost::asio::buffer(data, BUFFER_SIZE), err);
    if(!err) {
        GameInfo::PlayerCommandPackage playerCommandPackage;
        playerCommandPackage.ParseFromArray(data,length);
        //std::cout << playerCommandPackage.DebugString() << std::endl;
        context->InGameText.clear();
        for(int Index = 0; Index < 3; Index++) {
            context->InGameText.push_back(playerCommandPackage.messages()[Index]);
        }
        // set player commands
        for(int Index = 1; Index <= context->roomInfo.size(); Index++) {
              GameInfo::PlayerCommandRequest playerCommand = playerCommandPackage.dplayercommand()[Index - 1];
              int playerNumber = playerCommand.playernum();

              context->DPlayerCommands[playerNumber].DAction = static_cast <EAssetCapabilityType> (playerCommand.daction());
              context->DPlayerCommands[playerNumber].DTargetNumber = static_cast <EPlayerNumber> (playerCommand.dtargetnumber());
              context->DPlayerCommands[playerNumber].DTargetType = static_cast <EAssetType> (playerCommand.dtargettype());
              context->DPlayerCommands[playerNumber].DTargetLocation.X(playerCommand.dtargetlocation().dx());
              context->DPlayerCommands[playerNumber].DTargetLocation.Y(playerCommand.dtargetlocation().dy());

              std::list<std::weak_ptr<CPlayerAsset> > DActors;
              // add actors using umap
              for(int i = 0; i < playerCommand.dactors().size(); i++) {
                  DActors.push_back(CPlayerData::umap[playerCommand.dactors()[i]]);
              }
              context->DPlayerCommands[playerNumber].DActors = DActors;
        }
    }
}

void Client::StartUpdateRoomList(std::shared_ptr<CApplicationData> context) {
    io_service.reset();
    UpdateRoomList(context);
}

void Client::StartUpdateRoomInfo(std::shared_ptr<CApplicationData> context) {
    io_service.reset();
    UpdateRoomInfo(context);
}

// Close the conenction fromm server
void Client::CloseConnection(){
    SendMessage("Back");
    socket.close();
}
