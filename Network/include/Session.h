#ifndef SESSION_H
#define SESSION_H
#include <string>
#include <memory>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <boost/asio.hpp>

class User;

class Session
{
    // pure virtual class for differnet session/states
    public:
        virtual ~Session() {}
        virtual void DoRead(std::shared_ptr<User>  userPtr) = 0;
        virtual void DoWrite(std::shared_ptr<User>  userPtr) = 0;
        virtual void Start(std::shared_ptr<User>  userPtr) = 0;
};

#endif
