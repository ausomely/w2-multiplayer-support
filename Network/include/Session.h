#ifndef SESSION_H
#define SESSION_H
#include <string>
#include <memory>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <boost/asio.hpp>

#include "User.h"

class Session
{
    public:
        virtual ~Session() {}
        //virtual std::string GetName() = 0;
        virtual void DoRead(User_ptr UserPtr) = 0;
        virtual void DoWrite(User_ptr UserPtr) = 0;
        virtual void Start(User_ptr UserPtr) = 0;
};

#endif
