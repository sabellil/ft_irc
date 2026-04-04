#include "../include/Helpers.hpp"
#include "../include/Server.hpp"
#include "../include/User.hpp"
#include "../include/Message.hpp"
#include "../include/Channel.hpp"


User::User(int fd) : _fd(fd), _nick(""), _username(""), _realname(""), _hasPass(false), _hasNick(false), _hasUser(false), _registered(false), _shouldDisconnect(false), _inbuf(""), _outbuf("") {
    std::cout << "--> User created for fd : " << _fd << std::endl;
}

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

bool User::isRegistered() const
{
    return _registered;
}

void User::setRegistered(bool value)
{
    _registered = value;
}

bool User::hasNick() const
{
    return _hasNick;
}

bool User::hasPass() const
{
    return _hasPass;
}

void User::setHasPass(bool value)
{
    _hasPass = value;
}

bool User::shouldDisconnect() const
{
    return _shouldDisconnect;
}

void User::setShouldDisconnect(bool value)
{
    _shouldDisconnect = value;
}

const std::set<Channel*>& User::getChannels() const
{
    return _channels;
}

void User::addChannel(Channel* channel)
{
    _channels.insert(channel);
}

void User::removeChannel(Channel* channel)
{
    _channels.erase(channel);
}