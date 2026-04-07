#include "../include/Server.hpp"
#include "../include/User.hpp"
#include "../include/Message.hpp"
#include "../include/Colors.hpp"
#include "../include/Channel.hpp"

#include <set>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <cstring> //memset
#include <sys/socket.h>//accept recv
#include <sys/types.h>//
#include <netdb.h>
#include <fcntl.h> //manip des fd (get ou set options)
#include <arpa/inet.h> //ai_family

Server::Server(char * raw_port, const std::string& password)
: _raw_port(raw_port),
  _port(std::atoi(raw_port)),
  _password(password),
  _serverFd(-1)
{
}

Server::~Server() {}

void Server::initServerFd()
{

    struct addrinfo hints; 
    struct addrinfo * result; 

    memset(&hints, 0, sizeof(struct addrinfo));
    
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, _raw_port, &hints, &result) != 0 ) {
        throw std::logic_error("No port available. Cannot launch server. ");
    }

    _serverFd = socket(result->ai_family, result->ai_socktype, result->ai_protocol); 
    if (_serverFd < 0 ) {
        freeaddrinfo(result);
        throw std::logic_error("Fail socket. Cannot launch server. ");
    }

    if (fcntl(_serverFd, F_SETFL, O_NONBLOCK) < 0)
    {
        freeaddrinfo(result);
        close(_serverFd);
        throw std::logic_error("Fail fcntl. Cannot launch server. ");
    }

    
    int yes = 1;
    if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
    {
        freeaddrinfo(result);
        close(_serverFd);
        throw std::logic_error("Fail setsockopt. Cannot launch server.");
    }
    if (bind(_serverFd, result->ai_addr, result->ai_addrlen) < 0 ) {
        freeaddrinfo(result);
        close(_serverFd);
        throw std::logic_error("Fail bind. Cannot launch server. ");
    }
    freeaddrinfo(result);
    if (listen(_serverFd, 10) < 0 )
    {
        close(_serverFd);
        _serverFd = -1;
        throw std::logic_error("Fail listen. Cannot launch server. ");
    }
}

void Server::run()
{
    std::cout << GREEN "Server started on port " << this->_port << RESET << std::endl;
    g_run = 1;
    initServerFd();

    pollfd pfd_server = {this->_serverFd, POLLIN, 0};
    _pollFds.push_back(pfd_server);

    while (g_run == 1)
    {
        int pollReturn = poll(&_pollFds[0], _pollFds.size(), 2000);
        if (pollReturn < 0)
        {
            if (g_run == 0)
                break;
            std::cerr << "poll() failed" << std::endl;
            break;
        }
        if (pollReturn == 0)
            continue;

        for (size_t i = 0; i < _pollFds.size(); ++i)
        {
            int fd = _pollFds[i].fd;
            short revents = _pollFds[i].revents;

            if (revents == 0)
                continue;

            if (revents & (POLLERR | POLLHUP | POLLNVAL))
            {
                if (fd == _serverFd)
                {
                    std::cerr << "server socket error" << std::endl;
                    g_run = 0;
                    break;
                }
                disconnectClient(fd);
                --i;
                continue;
            }

            if (fd == _serverFd && (revents & POLLIN))
            {
                struct sockaddr_in client_addr;
                socklen_t addr_size = sizeof(client_addr);

                int client_fd = accept(_serverFd, (struct sockaddr *)&client_addr, &addr_size);
                if (client_fd < 0)
                {
                    std::cerr << "accept() failed" << std::endl;
                    continue;
                }

                if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1)
                {
                    close(client_fd);
                    std::cerr << "cannot setup socket as nonblock" << std::endl;
                    continue;
                }

                struct pollfd newFdToPoll;
                newFdToPoll.fd = client_fd;
                newFdToPoll.events = POLLIN;
                newFdToPoll.revents = 0;
                _usersByFd[client_fd] = new User(client_fd);
                _pollFds.push_back(newFdToPoll);
            }
            else
            {
                if (revents & POLLIN)
                {
                    size_t oldSize = _pollFds.size();
                    onClientRead(fd);
                    if (_pollFds.size() < oldSize)
                    {
                        --i;
                        continue;
                    }
                }

                if (_usersByFd.count(fd) && (revents & POLLOUT))
                    flushClientOutput(fd);
            }
        }
    }

    for (size_t i = 0; i < _pollFds.size();)
    {
        int fd = _pollFds[i].fd;

        if (fd == _serverFd)
        {
            ++i;
            continue;
        }

        disconnectClient(fd);
    }

    close(_serverFd);
    _pollFds.clear();

    std::cerr << RED "Server stopped" RESET << std::endl;
}


void Server::onClientRead(int clientFd)
{
    char buffer[4096];

    int bytesRead = recv(clientFd, buffer, sizeof(buffer), 0);
    if (bytesRead == 0)
    {
        disconnectClient(clientFd);
        return;
    }
    if (bytesRead < 0)
    {
        std::cout << "ERROR: recv failed" << std::endl;
        return;
    }
    std::map<int, User*>::iterator it = _usersByFd.find(clientFd);
    if (it == _usersByFd.end() || it->second == NULL)
    {
        return;
    }
    User* user = it->second;
    user->inbuf().append(buffer, bytesRead);
    processInputBuffer(*user);
    if (_usersByFd.count(clientFd) && user->shouldDisconnect())
        disconnectClient(clientFd);
}
 
void    Server::disconnectClient(int clientFd)
{
    std::map<int, User*>::iterator userIt = _usersByFd.find(clientFd);
    if (userIt == _usersByFd.end())
        return;
    User* user = userIt->second;
    if (!user->getNick().empty())
        _usersByNick.erase(user->getNick());
    for (std::map<std::string, Channel*>::iterator chanIt = _channels.begin(); chanIt != _channels.end(); )
    {
        Channel* channel = chanIt->second;
        if (channel->hasUser(user))
        {
            std::string quitMsg = ":" + user->getNick() + "!" + user->getUsername() + "@localhost QUIT :Client Quit";
            const std::set<User*>& users = channel->getUsers();
            for (std::set<User*>::const_iterator it = users.begin(); it != users.end(); ++it)
            {
                if (*it != user)
                    sendToClient(**it, quitMsg);
            }
            channel->removeUser(user);
            user->removeChannel(channel);
        }
        if (channel->getUsers().empty())
        {
            delete channel;
            std::map<std::string, Channel*>::iterator toErase = chanIt++;
            _channels.erase(toErase);
        }
        else
        {
            ++chanIt;
        }
    }
    for (std::vector<pollfd>::iterator it = _pollFds.begin(); it != _pollFds.end(); ++it)
    {
        if (it->fd == clientFd)
        {
            _pollFds.erase(it);
            break;
        }

    }
    close(clientFd);
    _usersByFd.erase(userIt);
    delete user;
    // std::cerr << YELLOW "--> Client/fd " << clientFd << " Disconnected !" RESET << std::endl;
}

void Server::processInputBuffer(User& user)
{
    std::string& buf = user.inbuf();
    std::string line;

    while (true)
    {
        size_t pos = buf.find("\n");
        if (pos == std::string::npos)
            break;

        line = buf.substr(0, pos);
        buf.erase(0, pos + 1);

        if (!line.empty() && line[line.size() -1] == '\r')
            line.erase(line.size() - 1);
        
        Message msg;
        if (!msg.parse(line))
            continue;

        dispatchCommand(user, msg);
        if (user.shouldDisconnect())
            return;
    }
}

