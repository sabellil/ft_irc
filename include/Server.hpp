
#include <string>
#include <vector>
#include <map>
#include <poll.h>

class User;
class Channel;
class Message;

class Server {
public:
    Server(int port, std::string& password);
    ~Server();

    void run();

private:
    int         _port;
    std::string _password;
    bool        _running;

    int                 _serverFd;
    std::vector<pollfd> _pollFds;//contient fd, events reventd

    std::map<int, User*>               _usersByFd;
    // std::map<std::string, User*>      _usersByNick;
    std::map<std::string, Channel*>   _channels;
};