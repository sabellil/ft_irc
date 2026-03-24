#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <map>
#include <poll.h>

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
    void                              disconnectClient(int clientFd);//si rien a lire = deconnexion
    void                              processInputBuffer(User& user);
    void                              handleNICK(User&, const Message&);//TEMPORAIREMENT EN PUBLIC pour tester avec main dedie
    void                              dispatchCommand(User& user, const Message& msg);//TEMPORAIREMENT EN PUBLIC
    void                              tryRegister(User& user);
    void                              sendToClient(User& user, const std::string& message);
    

private:    
    char *                            _raw_port;
    int                               _port;
    std::string                       _password;

    bool                              _running;
    int                               _serverFd;
    std::vector<pollfd>               _pollFds;//contient fd, events reventd
    std::map<int, User*>              _usersByFd;
    std::map<std::string, User*>      _usersByNick;
    std::map<std::string, Channel*>   _channels;
    //target?

    std::string                       getClientName(const User& user) const;
    void                              handlePASS(User&, const Message&);
    void                              handleUSER(User&, const Message&);
    void                              handleJOIN(User&, const Message&);
    void                              handlePRIVMSG(User&, const Message&);
    void                              handleKICK(User&, const Message&);
    void                              handleINVITE(User&, const Message&);
    void                              handleTOPIC(User&, const Message&);
    void                              handleMODE(User&, const Message&);
    void                              handleUnknown(User&, const Message&);
    bool                              requireRegistered(User& user);
    void                               initServerFd();
    
    

};

#endif
