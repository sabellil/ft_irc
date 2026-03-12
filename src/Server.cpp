#include "../include/Server.hpp"
#include "../include/User.hpp"
#include "../include/Message.hpp"
#include "../include/Errors.hpp"

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
        handlePASS(user, msg);//TODO
    else if (cmd == "NICK")
        handleNICK(user, msg);//OK
    else if (cmd == "USER")
        handleUSER(user, msg);//OK mais dependant de handlePass
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
    if (user.isRegistered())
    {
        std::cout << "ERROR: already registered!" << std::endl;
        return;
    }
    if (msg._params.empty())
    {
        std::cout << "ERROR: PASS requires a password, duh!!" << std::endl;
        return;
    }
    if (msg._params[0] != _password)
    {
        std::cout << "ERROR: wrong password!!!!! Try again" << std::endl;
        return;
    }
    user.setHasPass(true);
    std::cout << "PASSS accepted!" << std::endl;
    tryRegister(user);
}

void Server::handleNICK(User& user, const Message& msg)
{
    if (msg._params.empty())
    {
        std::cout << ERR_NONICKNAMEGIVEN << " No nickname given" << std::endl;//msg temporiare, on devra renvoyer un un msg au clent via la socket plus tard pour rnevoyer :ircserv 431 * :No nickname given
        return;
    }

    const std::string& newNick = msg._params[0];
    if (_usersByNick.count(newNick))
    {
        std::cout << ERR_NICKNAMEINUSE << " Nickname already in use" << std::endl;
        return;
    }

    if (!user.getNick().empty())//si le client a deja un username on l'erase proprement
        _usersByNick.erase(user.getNick());

    user.setNick(newNick);
    user.setHasNick(true);
    _usersByNick[newNick] = &user;//enregistrer que ce nouveau nickname appartient a cet user la
    std::cout << "New nick set to: " << user.getNick() << std::endl;
    tryRegister(user);
}

void Server::handleUSER(User& user, const Message& msg)
{
    if (msg._params.size() < 3 || msg._trailing.empty())
    {
        std::cout << "ERROR: USER needs username, mode, unused and realname" << std::endl;//TODO 1 trouver les bons msg d'erreur pour renvoyer depuis la socket proprement 
        return;
    }
    if (user.hasUser())
    {
        std::cout << "ERROR: USER already set" << std::endl;
        return;
    }
    user.setUsername(msg._params[0]);
    user.setRealname(msg._trailing);
    user.setHasUser(true);

    std::cout << "USER set to: " << user.getUsername() << std::endl;
    std::cout << "REALNAME set to: " << user.getRealname() << std::endl;

    tryRegister(user);
}

void Server::tryRegister(User& user)
{
    if (user.isRegistered())
        return;
    if (!user.hasPass())
        return;
    if (!user.hasNick())
        return;
    if (!user.hasUser())
        return;
    user.setRegistered(true);
    std::cout << "User registered:" << user.getNick() << "( "<< user.getUsername() <<")" << std::endl;
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
- handlePass
- tryRegister (test de pass nick et user)

*/
