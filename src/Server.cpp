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

    int bytesRead = recv(clientFd, buffer, sizeof(buffer), 0);
    if (bytesRead == 0)//le client est deconnecte
    {
        std::cout << "Client disconnected" << std::endl;
        // TODO: disconnectClient(clientFd);
        return;
    }
    if (bytesRead < 0)//erreur pendant la lecture de recv
    {
        std::cout << "ERROR: recv failed" << std::endl;
        return;
    }
    std::map<int, User*>::iterator it = _usersByFd.find(clientFd);
    if (it == _usersByFd.end() || it->second == NULL)//clientFd pas reconnu ou pas associe a un User valide
    {
        std::cout << "ERROR: unknown clientFd in onClientRead" << std::endl;
        return;
    }

    User* user = it->second;
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
        sendToClient(user, ":ircserv 462 " + getClientName(user) + " :You may not reregister");
        return;
    }
    if (user.hasPass())
    {
        std::cout << "ERROR: PASS already set." << std::endl;
        sendToClient(user, ":ircserv 462 " + getClientName(user) + " :You may not reregister");
        return;
    }
    if (msg._params.empty())
    {
        std::cout << "ERROR: No password given!" << std::endl;
        sendToClient(user, ":ircserv 461 " + getClientName(user) + " PASS :Not enough parameters");        return;
    }
    if (msg._params[0] != _password)
    {
        std::cout << "ERROR: wrong password!!!!! Try again" << std::endl;
        sendToClient(user, ":ircserv 464 " + getClientName(user) + " :Password incorrect");
        return;
    }
    user.setHasPass(true);
    std::cout << "PASS accepted!" << std::endl;
    tryRegister(user);
}

void Server::handleNICK(User& user, const Message& msg)
{
    if (msg._params.empty() || msg._params[0].empty())
    {
        std::cout << "ERROR: No nickname given" << std::endl;
        sendToClient(user, ":ircserv 431 " + getClientName(user) + " :No nickname given");
        return;
    }

    const std::string& newNick = msg._params[0];

    if (user.getNick() == newNick)
        return;

    if (_usersByNick.count(newNick))
    {
        std::cout << ERR_NICKNAMEINUSE << "ERROR: Nickname already in use" << std::endl;
        sendToClient(user, ":ircserv 433 " + getClientName(user) + " " + newNick + " :Nickname is already in use");        return;
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
    if (user.isRegistered())
    {
        std::cout << "ERROR: USER already registered. " << std::endl;
        sendToClient(user, ":ircserv 462 " + getClientName(user) + " :You may not reregister");
        return;
    }
    if (user.hasUser())
    {
        std::cout << "ERROR: USER already set" << std::endl;
        sendToClient(user, ":ircserv 462 " + getClientName(user) + " :You may not reregister");
        return;
    }
    if (msg._params.size() < 3 || msg._trailing.empty())
    {
        std::cout << "ERROR: USER needs username, mode, unused and realname" << std::endl;//TODO 1 trouver les bons msg d'erreur pour renvoyer depuis la socket proprement 
        sendToClient(user, ":ircserv 461 " + getClientName(user) + " :Not enough parameters");
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
    sendToClient(user, ":ircserv 001 " + getClientName(user) + " :Welcome to the IRC server");
}

void Server::handleJOIN(User& user, const Message& msg)
{
    if (!requireRegistered(user))
        return;

    if (msg._params.empty())
    {
        sendToClient(user, ":ircserv 461 " + user.getNick() + " JOIN :Not enough parameters\r\n");
        return; 
    }
    const std::string& channelName = msg._params[0];

    if (channelName.empty() || channelName[0] != '#')
    {
        sendToClient(user, ":ircserv 476 " + user.getNick() + " " + channelName + " :Bad Channel Mask\r\n");        return; 
    }

    Channel* channel;

    if (_channels.count(channelName) == 0)//si channel n'existe pas on le cree
    {
        channel = new Channel(channelName);
        _channels[channelName] = channel;
    }
    else//sinon on recupere celui qui existe deja
    {
        channel = _channels[channelName];
    }

    if (channel->hasUser(&user))
        return;

    channel->addUser(&user);
    sendToClient(user, ":" + user.getNick() + "!~" + user.getUsername() + "@localhost JOIN :" + channelName + "\r\n");
}

void Server::handlePRIVMSG(User& user, const Message& msg)
{
    if (!requireRegistered(user))
        return;
    if (msg._params.empty() || msg._trailing.empty())
    {
        sendToClient(user, ":ircserv 461 " + user.getNick() + " PRIVMSG: Not enough parameters\r\n");
        return;
    }
    const std::string& target = msg._params[0];
    const std::string& text = msg._trailing;

    //ligne renvoyee au user target
    std::string fullMsg= ":" + user.getNick() + "!" + user.getUsername() + "@localhost PRIVMSG " + target + ":" + text + "\r\n";
    //Verifier si trailing vide ou si param vide
    //Assigner la target
    //Assigner le msg
    //Variable fullMsg a remplir
        // Cas 1 message a un user specifique
        // Cas 2 message sur un canal
        // Cas 3 cas inconnu 
}

/*
Rappel process:
Le client irssi tape --> /msg mike hi whats up
irssi envoie au serveur irc (MOI) --> PRIVMSG make :hi whats up
je renvoie au client irssi --> :sara!user@host PRIVMSG mike: hi whats up
*/

void Server::handleKICK(User& user, const Message& msg)
{
    if (!requireRegistered(user))
        return;
    (void)msg;
}

void Server::handleINVITE(User& user, const Message& msg)
{
    if (!requireRegistered(user))
        return;
    (void)msg;
}

void Server::handleTOPIC(User& user, const Message& msg)
{
    if (!requireRegistered(user))
        return;
    (void)msg;
}

void Server::handleMODE(User& user, const Message& msg)
{
    if (!requireRegistered(user))
        return;
    (void)msg;
}

void Server::handleUnknown(User& user, const Message& msg)
{
    sendToClient(user, ":ircserv 421 " + getClientName(user) + " " + msg._command + " :Unknown command");
}

std::string Server::getClientName(const User& user) const//gestion propre du renvoi de msg is suer pas registered
{
    if (user.getNick().empty())
    {
        return "*";
    }
    return user.getNick();
}

void Server::sendToClient(User& user, const std::string& message)
{
    std::string fullMessage = message + "\r\n";
    send(user.getFd(), fullMessage.c_str(), fullMessage.size(), 0);
}

bool Server::requireRegistered(User & user)
{
    if (!user.isRegistered())
    {
        sendToClient(user, ":ircserv 464511 " + getClientName(user) + " :You have not registered yet");
        return false;
    }
    return true;
}

/*
TO DO SEMAINE:
- Systeme de renvoi des messages via la socket (fonction sendtouser() ?) 
- Tous les handlers

*/
