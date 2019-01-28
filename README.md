# ECS160 MultiPlayer Support

Please note that work on the multiplayer support for the game is still ongoing and not all features may be implemented at this time. 


## General:
This repository contains the code for multiplayer support for the game. For Linux, the client code is included in src and include along with the rest of the game code. Code for the server is in Network. If you are looking for information on how to run the current Linux version of the multiplayer game, go to the Setup section. If you are looking for information on how the multiplayer support interfaces with the rest of the game code, go to the Interface section. If you are looking for information on how to start or join a multiplayer game, go to the Playing section.
 

## Setup:

The following setup must take place in order to compile and run the multiplayer games.

### Protobuf:
In order to run a multiplayer game, you must install protobuf on your machine. A link to the protobuf repository, along with instructions, can be found below: 

https://github.com/protocolbuffers/protobuf.git

After cloning this repository, navigate to the protobuf directory and run the following commands to install it to usr:

./configure —-prefix=/usr
make
make check
make install

If “make check” fails, skip this step and move on to “make install”. If you run into issues with disk space, you can use “fquota” to look at your space usage and possibly delete any unnecessary files or clear the cache. 

Depending on where you install protobuf, it might not be part of LD_LIBRARY_PATH. If you installed it to your home directory, you can use the following command to solve this problem:

LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~lib && export PKG_CONFIG_PATH=/home/usr/lib/pkgconfig && export CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:~/include

This command must be run on each terminal window you use, so for this reason we recommend that you add it to your .bashrc file so that you do not have to do this manually every time.

### Installing boost asio:
Boost asio is also necessary to run the multiplayer games. Information on boost Casio can be found here:

https://github.com/boostorg/asio.git


## Playing:

To play a multiplayer game, an account with email, username, and password must be created. This is done through the following site:

https://ecs160.herokuapp.com

Once an account has been created, enter your information in the game settings menu. Once this is correct, you can start playing a multiplayer game.

In order to play a multiplayer game, one of the players must choose to host the game. To host the game, click on the “host multiplayer game” button in the multiplayer menu. Then select the game  map and other options the same as when you create a single player game. Once you start the game, other players will be able to see and join your game. If you want to join a game, click on the “join multiplayer game” button on the multiplayer menu. A list of the games that are able to be joined along with their details will be listed. 

## Server Protocols:

### Multiplayer Protocol:
Our server implemented a TCP connection between the client (the game). Our server divided the whole process of exchanging information into several session. For now, we have LoginSession, where we exchange the user information such as username and password and authenticate the information with web server, and InGameSession, where we exchange the player commands, and store them locally and remotely.

For exchanging different information such as username, password and player commands, we used protobuf to package the informaton and serialize it to a stream buffer in the form of binary string. Then we used asynchronous operations to send the information on the client side and receive them on the server side.

## Web Server Protocol:
To communicate with the web server and authenticate the credentials of our users, we sent a Post request via JSON objects and received the response. Since we only cared about if the authentication is a success or not, we only extract the status code from the socket received.


## Interfaces:
When the user clicks on the multiplayer menu, their username and password is taken from the game and sent to the web server for authentication. 

Each individual game sends the actions that that user is doing to the server, which then forwards them to the other players in that game. All users then receive and process the actions from the other players and use this to update the game state.

The main class that handles the multiplayer communication is Clientcpp, an instance of which is part of the game data. This class is called from the MainMenuMode.cpp to authenticate the user information and establish a connection to the server, and is also used BattleMode.cpp whenever an action takes place to send the action information to the server. 

