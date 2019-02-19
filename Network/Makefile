CXX=g++
PROTOC=~/bin/protoc
CFLAGS   +=  -Wall
CPPFLAGS += -std=c++11
BOOSTFLAGS += -lboost_system -lprotobuf
PROTOCFLAGS += `pkg-config --cflags --libs protobuf`

INC_DIR = ./include
SRC_DIR = ./src
OBJ_DIR = ./obj

INCLUDE  += -I $(INC_DIR)

SERVER_NAME = boostServer
SERVER_OBJS = $(OBJ_DIR)/main.o \
        $(OBJ_DIR)/User.o \
	      $(OBJ_DIR)/Lobby.o \
        $(OBJ_DIR)/LoginSession.o \
				$(OBJ_DIR)/InGameSession.o \
				$(OBJ_DIR)/FindGameSession.o \
				$(OBJ_DIR)/HostGameSession.o \
				$(OBJ_DIR)/InRoomSession.o \
				$(OBJ_DIR)/GameRoom.o \
        $(OBJ_DIR)/Server.o \
        $(OBJ_DIR)/LoginInfo.pb.o \
				$(OBJ_DIR)/RoomInfo.pb.o \
        $(OBJ_DIR)/GameInfo.pb.o \
        $(OBJ_DIR)/AcceptedSession.o

all: directories $(SERVER_NAME)

$(SERVER_NAME): $(SERVER_OBJS)
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(SERVER_OBJS) -o $(SERVER_NAME) $(BOOSTFLAGS) $(PROTOCFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(INCLUDE) -c $< -o $@

$(OBJ_DIR)/%.pb.o: $(SRC_DIR)/%.pb.cc
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(INCLUDE) -c $< -o $@

.PHONY: directories proto
directories:
	mkdir -p $(OBJ_DIR)

proto:
	-rm ./src/*.pb.cc ./include/*.pb.h ../src/*.pb.cc ../include/*.pb.h 2>/dev/null
	$(PROTOC) --cpp_out=. --proto_path=./protobuf/ ./protobuf/*.proto
	cp ./*.pb.cc ./src
	cp ./*.pb.h ./include
	cp ./*.pb.cc ../src
	cp ./*.pb.h ../include
	rm ./*.pb*
clean::
	-rm -f $(SERVER_OBJS) $(SERVER_NAME)
.PHONY: clean
