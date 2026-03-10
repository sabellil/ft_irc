#include "../include/User.hpp"

User::User(int fd) : _fd(fd), _nick(""), _username(""), _hasPass(false), _hasNick(false), _hasUser(false), _registered(false), _inbuf(""), _outbuf("") {}

User::~User() {}

int User::getFd() const
{
    return _fd;
}

const std::string User::getNick() const
{
    return _nick;
}

const std::string User::getUsername() const
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