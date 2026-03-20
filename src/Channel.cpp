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

const std::set<User*>& Channel::getUsers() const
{
    return _users;
}


bool Channel::isOperator(User * user) const
{
    return _operators.count(user) > 0;
}

void Channel::addOperator(User* user)
{
    _operators.insert(user);
}

void Channel::removeOperator(User* user)
{
    _operators.erase(user);
}

const std::set<User*?> Channel::getOperators() const
{
    return _operators;
}