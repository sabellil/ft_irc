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

//Lit les octets envoyes par recv(), les ajoute au buffer puis declenche traitement du parsing
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

//Analyse _inbuf de l'utilisateur pour extraire chaque lgien complete et les renvoie au parseur IRC
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
        if (!msg.parse(line))
            continue;

        dispatchCommand(user, msg);
    }
}


void Server::dispatchCommand(User& user, const Message& msg)
{
    const std::string& cmd = msg._command;

    if (cmd == "PASS")
        handlePASS(user, msg);
    else if (cmd == "NICK")
        handleNICK(user, msg);
    else if (cmd == "USER")
        handleUSER(user, msg);
    else if (cmd == "JOIN")
        handleJOIN(user, msg);
    else if (cmd == "PRIVMSG")
        handlePRIVMSG(user, msg);
    else if (cmd == "KICK")
        handleKICK(user, msg);
    else if (cmd == "INVITE")
        handleINVITE(user, msg);
    else if (cmd == "TOPIC")
        handleTOPIC(user, msg);
    else if (cmd == "MODE")
        handleMODE(user, msg);
    else
        handleUnknown(user, msg);
}

void Server::handlePASS(User& user, const Message& msg)
{
    (void)user;
    (void)msg;
}

void Server::handleNICK(User& user, const Message& msg)
{
    if (msg._params.empty())
    {
        std::cout << "NICK: missing parameter" << std::endl;
        return;
    }

    const std::string& newNick = msg._params[0];
    user.setNick(newNick);
    user.setHasNick(true);
    std::cout << "New nick set to: " << user.getNick() << std::endl;
}

void Server::handleUSER(User& user, const Message& msg)
{
    (void)user;
    (void)msg;
}

void Server::handleJOIN(User& user, const Message& msg)
{
    (void)user;
    (void)msg;
}

void Server::handlePRIVMSG(User& user, const Message& msg)
{
    (void)user;
    (void)msg;
}

void Server::handleKICK(User& user, const Message& msg)
{
    (void)user;
    (void)msg;
}

void Server::handleINVITE(User& user, const Message& msg)
{
    (void)user;
    (void)msg;
}

void Server::handleTOPIC(User& user, const Message& msg)
{
    (void)user;
    (void)msg;
}

void Server::handleMODE(User& user, const Message& msg)
{
    (void)user;
    (void)msg;
}

void Server::handleUnknown(User& user, const Message& msg)
{
    (void)user;
    (void)msg;
}

/*
TO DO NEXT:
- handleNick
- handleUser
- handlePass
- tryRegister (test de pass nick et user)

*/