#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "RoomInfo.pb.h"

using boost::asio::ip::tcp;

#define BUFFER_SIZE 1024

class CApplicationData;

class Client: public std::enable_shared_from_this<Client>
{
    public:
    // members
        boost::asio::io_service io_service;
        tcp::socket socket;
        tcp::resolver resolver;
        char data[BUFFER_SIZE];

    // member functions
        Client();
        bool Connect(std::shared_ptr<CApplicationData> context);
        bool SendLoginInfo(std::shared_ptr<CApplicationData> context);
        void SendGameInfo(std::shared_ptr<CApplicationData> context);
        void SendRoomInfo(std::shared_ptr<CApplicationData> context);
        void UpdateRoomList(RoomInfo::RoomInfoPackage* roomList);
        void UpdateRoomInfo(RoomInfo::RoomInformation* roomInfo);
        void SendMessage(std::string message);
        RoomInfo::RoomInformation GetRoomInfo();
        void GetGameInfo(std::shared_ptr<CApplicationData> context);
        void StartUpdateRoomList(RoomInfo::RoomInfoPackage* roomList);
        void StartUpdateRoomInfo(RoomInfo::RoomInformation* roomInfo);
        void CloseConnection();
};
#endif
