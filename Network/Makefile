CXX=g++

CFLAGS   +=  -Wall

BOOSTFLAGS += -L /usr/lib/x86_64-linux-gnu/libboost_system.a -lboost_system
SERVER_NAME = boostServer
all: $(SERVER_NAME)

#variable approach was acting up
$(SERVER_NAME): boostServer.cpp
	g++ boostServer.cpp -L /usr/lib/x86_64-linux-gnu/libboost_system.a -lboost_system -o boostServer

clean::
	rm boostServer 

.PHONY: clean
