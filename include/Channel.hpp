#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#pragma once 

#include <string>
#include <set>

class User;

class Channel
{
    public:
    //nom, liste des users presents, fonction ajout user, savoir is un user ets deja present, retirer un user

    private:
    Channel();
    Channel(const std::string& name);
    ~Channel();
};


#endif