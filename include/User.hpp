#ifndef USER_HPP
#define USER_HPP

#include <string>

class User
{
private:
    int                 _fd;//ou _clientFd ?

    std::string         _nick;
    std::string         _username;

    bool                _hasPass;
    bool                _hasNick;
    bool                _hasUser;
    bool                _registered;

    std::string         _inbuf;//s'assurer d'aller jusqu'au \r\n et d'avoir toute la ligne

public:
    User(int fd);
    ~User();

    std::string& inbuf() { return _inbuf; }//pour retourner le cumul des fragments de ligne recup par recv()
};

#endif