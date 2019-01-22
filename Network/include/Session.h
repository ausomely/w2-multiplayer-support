#ifndef SESSION_H
#define SESSION_H
#include <string>
#include <memory>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <boost/asio.hpp>

class Session
{
    public:
        virtual ~Session() {}
        virtual std::string GetName() = 0;
};

typedef std::shared_ptr<Session> Session_ptr;

#endif
