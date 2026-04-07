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
    void                              onClientRead(int clientFd);
    void                              acceptClient();
    void                              disconnectClient(int clientFd);
    void                              processInputBuffer(User& user);
    void                              handleNICK(User&, const Message&);
    void                              dispatchCommand(User& user, const Message& msg);
    void                              tryRegister(User& user);
    void                              sendToClient(User& user, const std::string& message);
    void                              handlePING(User& user, const Message& msg);

private:    
    char *                            _raw_port;
    int                               _port;
    std::string                       _password;
    bool                              _running;
    int                               _serverFd;
    std::vector<pollfd>               _pollFds;
    std::map<int, User*>              _usersByFd;
    std::map<std::string, User*>      _usersByNick;
    std::map<std::string, Channel*>   _channels;

    std::string                       getClientName(const User& user) const;
    void                              handlePASS(User&, const Message&);
    void                              handleUSER(User&, const Message&);
    void                              handleJOIN(User&, const Message&);
    void                              handlePRIVMSG(User&, const Message&);
    void                              handleKICK(User&, const Message&);
    void                              handleINVITE(User&, const Message&);
    void                              handleTOPIC(User&, const Message&);
    void                              handleMODE(User&, const Message&);
    void                              handlePART(User&, const Message&);
    void                              handleUnknown(User&, const Message&);
    bool                              requireRegistered(User& user);
    void                              initServerFd();
    void                              flushClientOutput(int clientFd);

};

#endif
