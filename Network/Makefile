CXX=g++
CFLAGS   +=  -Wall
CPPFLAGS += -std=c++11
BOOSTFLAGS += -lboost_system

INC_DIR = ./include
SRC_DIR = ./src
OBJ_DIR = ./obj

INCLUDE  += -I $(INC_DIR)

SERVER_NAME = boostServer
SERVER_OBJS = $(OBJ_DIR)/main.o \
							$(OBJ_DIR)/User.o \
              $(OBJ_DIR)/Lobby.o \
							$(OBJ_DIR)/LoginSession.o \
							$(OBJ_DIR)/Server.o

all: $(SERVER_NAME)

#variable approach was acting up
$(SERVER_NAME): $(SERVER_OBJS)
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(BOOSTFLAGS) $(SERVER_OBJS) -o $(SERVER_NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(INCLUDE) -c $< -o $@

.PHONY: directories
directories:
	mkdir -p $(OBJ_DIR)

clean::
	-rm $(SERVER_OBJS) $(SERVER_NAME)

.PHONY: clean
