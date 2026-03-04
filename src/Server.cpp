#include "../include/Server.hpp"
#include "../include/User.hpp"
#include "../include/Message.hpp"

#include <iostream>
#include <sys/socket.h>//accept recv


Server::Server(int port, const std::string& password)
: _port(port),
  _password(password),
  _running(true),
  _serverFd(-1)
{
}

Server::~Server() {}

void Server::onClientRead(int clientFd)
{
    char buffer[4096];
    int bytesRead = recv(clientFd, buffer, sizeof(buffer), 0);//je recupere des octets envoyes par le client dans un buffer alloue d'une taille max de buffer, pas d'option
    if (bytesRead <= 0)
    {
        // disconnectClient(clientFd);TODO
        return;
    }
    User* user = _usersByFd[clientFd];
    user->inbuf().append(buffer, bytesRead);
    processInputBuffer(*user);
}

void Server::processInputBuffer(User& user)
{
    std::string& buf = user.inbuf();
    std::string line;

    while (true)
    {
        size_t pos = buf.find("\r\n");
        if (pos == std::string::npos)
            break;

        line = buf.substr(0, pos);
        buf.erase(0, pos + 2);
        Message msg;
        msg.parse(line);
        dispatchCommand(msg); TODO
    }
}
