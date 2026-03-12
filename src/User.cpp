#include "../include/User.hpp"

User::User(int fd) : _fd(fd), _nick(""), _username(""), _hasPass(false), _hasNick(false), _hasUser(false), _registered(false), _inbuf(""), _outbuf("") {}

User::~User() {}

int User::getFd() const
{
    return _fd;
}

std::string User::getNick() const
{
    return _nick;
}

std::string User::getUsername() const
{
    return _username;
}

void User::setNick(const std::string& nickname)
{
    _nick = nickname;
}

void User::setUsername(const std::string& username)
{
    _username = username;
}

void User::setHasNick(bool value)
{
    _hasNick = value;
}


bool User::hasUser() const
{
    return _hasUser;
}

void User::setHasUser(bool value)
{
    _hasUser = value;
}

void User::setRealname(const std::string& realname)
{
    _realname = realname;
}

std::string User::getRealname() const
{
    return _realname;
}

