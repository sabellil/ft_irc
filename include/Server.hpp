#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <map>
#include <poll.h>
#include <csignal>

extern volatile sig_atomic_t g_run;

class User;
class Channel;
class Message;

class Server {
public:
    Server(int port, const std::string& password);
    Server(char * raw_port, const std::string& password);
    ~Server();

    void                              run();

private:    
    char*                             _raw_port;
    int                               _port;
    std::string                       _password;
    bool                              _running;
    int                               _serverFd;
    std::vector<pollfd>               _pollFds;
    std::map<int, User*>              _usersByFd;
    std::map<std::string, User*>      _usersByNick;
    std::map<std::string, Channel*>   _channels;

    void                              onClientRead(int clientFd);
    void                              acceptClient();
    void                              disconnectClient(int clientFd);
    void                              processInputBuffer(User& user);
    std::string                       getClientName(const User& user) const;
    void                              initServerFd();
    void                              flushClientOutput(int clientFd);
    void                              tryRegister(User& user);
    void                              sendToClient(User& user, const std::string& message);
    void                              dispatchCommand(User& user, const Message& msg);
    
    void                              handleNick(User&, const Message&);
    void                              handlePass(User&, const Message&);
    void                              handleUser(User&, const Message&);
    void                              handleJoin(User&, const Message&);
    void                              handlePrivmsg(User&, const Message&);
    void                              handleKick(User&, const Message&);
    void                              handleInvite(User&, const Message&);
    void                              handleTopic(User&, const Message&);
    void                              handleMode(User&, const Message&);
    void                              handlePart(User&, const Message&);
    void                              handlePing(User& user, const Message& msg);
    void                              handleUnknown(User&, const Message&);
    bool                              requireRegistered(User& user);
};

#endif
