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
    public:
        virtual ~Session() {}
        //virtual std::string GetName() = 0;
        virtual void DoRead(std::shared_ptr<User>  UserPtr) = 0;
        virtual void DoWrite(std::shared_ptr<User>  UserPtr) = 0;
        virtual void Start(std::shared_ptr<User>  UserPtr) = 0;
};

#endif
