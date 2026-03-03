#include "../include/User.hpp"

User::User(int fd) : _fd(fd), _nick(""), _username(""), _hasPass(false), _hasNick(false), _hasUser(false), _registered(false), _inbuf("") {}

User::~User() {}