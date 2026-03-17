#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#pragma once 

#include <string>
#include <set>

class User;

class Channel
{
    public:
    Channel();
    Channel(const std::string& name);
    ~Channel();

    const std::string&      getName() const;
    bool                    hasUser(User* user) const;//un user est-il deja dans le channel?
    void                    addUser(User* user);
    void                    removeUser(User* user);
    const std::set<User*>&  getUsers() const;//recuperer tous les users d'un channel
    private:
    std::string             _name;
    std::set<User*>         _users;//set pour stocker mes users, eviter les doublons et verifier rqpidement si il est deja dans le channel

};


#endif