CXX=g++
CFLAGS   +=  -Wall
CPPFLAGS += -std=c++11
BOOSTFLAGS += -lboost_system

SERVER_NAME = boostServer
all: $(SERVER_NAME)

#variable approach was acting up
$(SERVER_NAME): boostServer.cpp
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(BOOSTFLAGS) boostServer.cpp -o $(SERVER_NAME)

clean::
	rm $(SERVER_NAME)

.PHONY: clean
