#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>

#define BUFFER_SIZE     256

class Client {

    /* TO DO */
    // friend classes to get access of the class info

    // potentially a Package class

    public:
    // members
        int SocketFileDescriptor;
        int PortNumber;
        struct sockaddr_in ServerAddress;
        struct hostent *Server;

        /* TO DO */
        /* potentially a flag or shared_ptr to indicate the client mode we have
           connection mode in which we tries to connect the clients
           game select mode in which we tries to select a game to player
           game create mode in which we tries to create a new game
           game playing mode in which we exchange game information */

    // member functions
        Client();
        bool Connect(std::string hostName, int portNumber);
        void SendMessage(std::string data);
        void CloseConnection();

    /* TO DO  */
        /* This function should connect to the web server for authentication
           of the username and probably the password?? */
        bool GetAuthentication();

        /* This function should send a request to the server for receiving
           a packet of information to the player.
           including list of connected clients, available games.  */
        void RequestGeneralInfoPacket();

        /* This function should send a packet of game information to the server
           so that the server can send to other clients as well as receiving info
           from other clients.
           such as player assets, commands. The parameter will be a class
           called Package we will write to encode info later on
           the parameter will probably be a shared_ptr point of the game context (not sure)*/
        void ExchangeGameInfoPacket();

        /* This functio changes the client mode in which we use to control what
           info we exchange with the server
           paremeter will be flag or shared_ptr instance of the available modes */
        void ChangeClientMode();

        /* some other functions to add */



};
#endif
