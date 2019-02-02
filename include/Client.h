#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>

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

    // member functions
        Client();
        bool Connect(std::shared_ptr<CApplicationData> context);
        bool SendLoginInfo(std::shared_ptr<CApplicationData> context);
        void SendGameInfo(std::shared_ptr<CApplicationData> context);
        void SendRoomInfo(std::shared_ptr<CApplicationData> context);
        //void GetRoomList(std::shared_ptr<CApplicationData> context,
          //  RoomInfo::RoomInfoPackage *roomList);
        RoomInfo::RoomInfoPackage GetRoomList(std::shared_ptr<CApplicationData> context);
        void UpdateRoomList(RoomInfo::RoomInfoPackage* roomList);
        void HandleUpdateRoomList(char* data, RoomInfo::RoomInfoPackage* roomList,
            const boost::system::error_code& err);
        void SendMessage(std::string message);
        void GetGameInfo(std::shared_ptr<CApplicationData> context);
        void CloseConnection();
};
#endif
