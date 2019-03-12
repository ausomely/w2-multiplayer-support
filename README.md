# ECS160 MultiPlayer Support

Please note that work on the multiplayer support for the game is still ongoing and not all features may be implemented at this time. 


## General:
This repository contains the code for multiplayer support for the game. For Linux, the client code is included in src and include along with the rest of the game code. Code for the server is in Network. If you are looking for information on how to run the current Linux version of the multiplayer game, go to the Setup section. If you are looking for information on how the multiplayer support interfaces with the rest of the game code, go to the Interface section. If you are looking for information on how to start or join a multiplayer game, go to the Playing section.
 

## Setup:

The following setup must take place in order to compile the multiplayer games.

### Protobuf:
In order to compile the game and our server code, you must install protobuf on your machine. A link to the protobuf repository, along with instructions, can be found below: 

https://github.com/protocolbuffers/protobuf.git

After cloning this repository, navigate to the protobuf directory and run the following commands to install it to usr:

./configure --prefix=/usr

make

make check

make install

If “make check” fails, skip this step and move on to “make install”. If you run into issues with disk space, you can use “fquota” to look at your space usage and possibly delete any unnecessary files or clear the cache. 

Depending on where you install protobuf, it might not be part of LD_LIBRARY_PATH. If you installed it to your home directory, you can use the following command to solve this problem:

LD_LIBRARY_PATH=$LD_LIBRARY_PATH:\~lib && export PKG_CONFIG_PATH=/home/usr/lib/pkgconfig && export CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:\~/include

This command must be run on each terminal window you use, so for this reason we recommend that you add it to your .bashrc file so that you do not have to do this manually every time.

### Installing boost asio:
Boost asio is also necessary to compile the game. Information on boost Casio can be found here:

https://github.com/boostorg/asio.git


## Playing:

To play a multiplayer game, an account with email, username, and password must be created. This is done through the following site:

https://ecs160.herokuapp.com

Once an account has been created, enter your information in the game settings menu. Once this is correct, you can start playing a multiplayer game.

In order to play a multiplayer game, one of the players must choose to host the game. To host the game, click on the “host multiplayer game” button in the multiplayer menu. Then select the game  map and other options the same as when you create a single player game. Once you start the game, other players will be able to see and join your game. If you want to join a game, click on the “join multiplayer game” button on the multiplayer menu. A list of the games that are able to be joined along with their details will be listed. 

## Server Protocols:

### Multiplayer Protocol (Probably Remove This):
Our server establishes a TCP connection between the client (the game). Our server divides the whole process of exchanging information into several sessions. For now, we have LoginSession, where we exchange the user information such as username and password and authenticate the information with web server, and InGameSession, where we exchange the player commands, and store them locally and remotely.

#### ***Login Session***:
Listens for connecting clients; establishes a TCP connection with them and attempts to authenticate that person with the web server. Upon successful authentication, the client is able to proceed to the multiplayer options menu. The server remembers the client; storing them on the active users list, and placing that user to ***Accepted Session***. Note that all of this is done asynchronously so the server does not hang when attempting to log a user on; allowing the server to continously listen and do other operations.

#### ***Accepted Session***:
The default lobby for a client to be in. The server essentially treats a user here as idle. Upon a user pressing "back" here, the connection will be ended and the user removed from the active users list.

#### ***Find Game Session***:
The session a user will be placed in if they choose to look up existing game rooms. The server will first send a list of active games and their details to the client when joining this session. Upon any changes to the list of active game rooms, the server will continue to broadcast these changes to the client so long as they remain in this session. Note that any game in progress is unjoinable, but will remain on the list of game rooms. Upon backing out, the user is placed back into ***Accepted Session***.

#### ***Host Game Session***:
The session a user will be placed in if they choose to host a new game on the server. In this session, a game room will be created on the server, and the details of the room will be available for people to see for anyone in ***Find Game Session***.

#### ***In Room Session***:
Once a player hosts a game room, or someone joins one, they switch into this session. As players continue to join, and changes in the state of a game room occur, the room is constantly updated and those changes are sent to anyone to see in ***Find Game Session***. Upon backing out of a game room, the user is placed back into accepted session.

#### ***In Game Session***:
Once all players in a room session are ready, and the host starts, all players in the game room are sent into ***In Game Session***. From here, this is where the gameplay actually happens. As the game is turn based, and all inputs of the user processed upon the game-cycle timer, at the end of each turn, all users inputs are ready, taken and added to a protobuf package, and then sent to the server. From here, on the next turn, the users will then do another move, send to the server that turns moves, and then finally recieve last turns moves and process those. The game is always one step behind; but this allows for everyone to stay in sync with one another and is a small price to pay to prevent the game from freezing each turn waiting for players to submit their packet to the server and then recieve the turns moves and process them. Upon the game ending, the results of the match are sent and processed by the web server, and the players exit back into ***Accepted Session***.

### Web Server Protocol
When a user connects to our game server, a login session is started to get authentication information from the user. Once our game server has the user’s credentials, we place them in a JSON format below.
```
{
	"user": {
		"email": "test2@ucdavis.edu",
		"password": "password"
	}
}
```
Then we convert it into a string and put the string into the HTTP POST request and send it to the web server. Since we only care about if the authentication is a success or not, we only extract the status code from the response, which should be 200 if succeeds. If authenticated, the user is placed in the “In Game” session (will change to primary lobby session).


## Interfaces:
When the user clicks on the multiplayer menu, their username and password is taken from the game and sent to the web server for authentication. 

Each individual game sends the actions that that user is doing to the server, which then forwards them to the other players in that game. All users then receive and process the actions from the other players and use this to update the game state.

The main class that handles the multiplayer communication is Client in Client.cpp, an instance of which is part of the game data. This class is called from the MainMenuMode.cpp to authenticate the user information and establish a connection to the server, and is also used BattleMode.cpp whenever an action takes place to send the action information to the server. 

### Network Interface design for other platforms: (subject to change)
![Current Design with the interface of finding games](Interface/FindGame.png?raw=true "Title")

![Current Design with the interface of waiting for a game to start](Interface/WaitInLobby.png?raw=true "Title")

