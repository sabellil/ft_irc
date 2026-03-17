#include "../include/Channel.hpp"
#include "../include/User.hpp"

Channel::Channel() : _name("")
{

}

Channel::Channel(const std::string& name) : _name(name)
{

}

Channel::~Channel()
{
}

const std::string& Channel::getName() const
{
    return _name;
}

bool Channel::hasUser(User* user) const
{
    return _users.count(user) > 0;
}

void Channel::addUser(User* user)
{
    _users.insert(user);
}

void Channel::removeUser(User* user)
{
    _users.erase(user);
}

//getuser TODO MERCREDI 1st thing