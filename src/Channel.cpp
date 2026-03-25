#include "../include/Channel.hpp"
#include "../include/User.hpp"

Channel::Channel() : _name("")
{

}

Channel::Channel(const std::string& name) : _name(name)
{

}
//A update? OUI avec les differents arguments de user limit true false, etc
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

const std::set<User*>& Channel::getOperators() const
{
    return _operators;
}

bool Channel::isInvited(User* user) const
{
    return _invitedUsers.find(user) != _invitedUsers.end();
}

bool Channel::isInviteOnly() const
{
    return _inviteOnly;
}

void Channel::addInvited(User* user)
{
    _invitedUsers.insert(user);
}

void Channel::setInviteOnly(bool value)
{
    _inviteOnly = value;
}

bool Channel::isTopicRestricted() const
{
    return _topicRestricted;
}

void Channel::setTopicRestricted(bool value)
{
    _topicRestricted = value;
}

bool Channel::hasKey() const
{
    return _hasKey;
}

const std::string& Channel::getKey() const
{
    return _key;
}

void Channel::setKey(const std::string& key)
{
    _hasKey = true;
    _key = key;
}

void Channel::removeKey()
{
    _hasKey = false;
    _key.clear();
}

bool Channel::hasUserLimit() const
{
    return _hasUserLimit;
}

int Channel::getUserLimit() const
{
    return _userLimit;
}

void Channel::setUserLimit(int limit)
{
    _hasUserLimit = true;
    _userLimit = limit;
}

void Channel::removeUserLimit()
{
    _hasUserLimit = false;
    _userLimit = 0;
}

// const std::string& Channel::getTopic() const
// {
//     return _topic;
// }

// void Channel::setTopic(const std::string& topic)
// {
//     _topic = topic;
// }
