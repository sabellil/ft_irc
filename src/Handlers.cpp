#include "../include/Server.hpp"
#include "../include/User.hpp"
#include "../include/Message.hpp"
#include "../include/Channel.hpp"

#include <set>
#include <iostream>
#include <sys/socket.h>


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
    if (user.isRegistered())
    {
        std::cout << "ERROR: already registered!" << std::endl;
        sendToClient(user, ":ircserv 462 " + getClientName(user) + " PASS :You may not reregister");
        return;
    }
    if (user.hasPass())
    {
        std::cout << "ERROR: PASS already set." << std::endl;
        sendToClient(user, ":ircserv 462 " + getClientName(user) + " PASS :You may not reregister");
        return;
    }
    if (msg._params.empty())
    {
        std::cout << "ERROR: No password given!" << std::endl;
        sendToClient(user, ":ircserv 461 " + getClientName(user) + " PASS :Not enough parameters");
        return;
    }
    if (msg._params[0] != _password)
    {
        std::cout << "ERROR: wrong password!!!!! Try again" << std::endl;
        sendToClient(user, ":ircserv 464 " + getClientName(user) + " PASS :Password incorrect");
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
        sendToClient(user, ":ircserv 431 " + getClientName(user) + " NICK :No nickname given");
        return;
    }

    const std::string& newNick = msg._params[0];

    if (user.getNick() == newNick)
        return;

    if (_usersByNick.count(newNick))
    {
        std::cout << "ERROR: Nickname already in use" << std::endl;
        sendToClient(user, ":ircserv 433 " + getClientName(user) + " " + newNick + " :Nickname is already in use");
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
    if (user.isRegistered())
    {
        std::cout << "ERROR: USER already registered. " << std::endl;
        sendToClient(user, ":ircserv 462 " + getClientName(user) + " USER :You may not reregister");
        return;
    }
    if (user.hasUser())
    {
        std::cout << "ERROR: USER already set" << std::endl;
        sendToClient(user, ":ircserv 462 " + getClientName(user) + " USER :You may not reregister");
        return;
    }
    if (msg._params.size() < 3 || msg._trailing.empty())
    {
        std::cout << "ERROR: USER needs username, mode, unused and realname" << std::endl;//TODO 1 trouver les bons msg d'erreur pour renvoyer depuis la socket proprement 
        sendToClient(user, ":ircserv 461 " + getClientName(user) + " USER :Not enough parameters");
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
        sendToClient(user, ":ircserv 461 " + user.getNick() + " JOIN :Not enough parameters");
        return;
    }

    const std::string& channelName = msg._params[0];

    if (channelName.empty() || channelName[0] != '#')
    {
        sendToClient(user, ":ircserv 476 " + user.getNick() + " " + channelName + " :Bad Channel Mask");
        return;
    }

    Channel* channel;
    
    bool    isNewChannel = false;

    if (_channels.count(channelName) == 0)
    {
        channel = new Channel(channelName);
        _channels[channelName] = channel;
        isNewChannel = true;
    }
    else
    {
        channel = _channels[channelName];
    }

    if (channel->hasUser(&user))
        return;

    channel->addUser(&user);
    if (isNewChannel)
        channel->addOperator(&user);

    std::string joinMsg = ":" + user.getNick() + "!" + user.getUsername() + "@localhost JOIN :" + channelName;

    const std::set<User*>& users = channel->getUsers();
    for (std::set<User*>::const_iterator it = users.begin(); it != users.end(); ++it)
    {
        sendToClient(**it, joinMsg);
    }

    std::string names;
    for (std::set<User*>::const_iterator it = users.begin(); it != users.end(); ++it)
    {
        if (!names.empty())
            names += " ";
        if (channel->isOperator(*it))
            name += "@";
        name += (*it)->getNick();
    }

    sendToClient(user, ":ircserv 353 " + user.getNick() + " = " + channelName + " :" + names);
    sendToClient(user, ":ircserv 366 " + user.getNick() + " " + channelName + " :End of /NAMES list");
}

void Server::handlePRIVMSG(User& user, const Message& msg)
{
    if (!requireRegistered(user))
        return;

    if (msg._params.empty() || msg._trailing.empty())
    {
        sendToClient(user, ":ircserv 461 " + user.getNick() + " PRIVMSG :Not enough parameters");
        return;
    }

    const std::string& target = msg._params[0];
    const std::string& text = msg._trailing;

    std::string fullMsg = ":" + user.getNick() + "!" + user.getUsername() + "@localhost PRIVMSG " + target + " :" + text;

    if (_usersByNick.count(target))
    {
        User* targetUser = _usersByNick[target];
        sendToClient(*targetUser, fullMsg);
        return;
    }

    if (target[0] == '#' && _channels.count(target))
    {
        Channel* channel = _channels[target];
        
        if (!channel->hasUser(&user))
        {
            sendToClient(user, ":ircserv 404 " + user.getNick() + " " + target + " :Cannot send to channel");
            return;
        }
        const std::set<User*>& users = channel->getUsers();
        for (std::set<User*>::const_iterator it = users.begin(); it != users.end(); ++it)
        {
            if (*it != &user)
                sendToClient(**it, fullMsg);
        }
        return;
    }

    sendToClient(user, ":ircserv 401 " + user.getNick() + " " + target + " PRIVMSG :No such nick/channel");
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
    ssize_t bytes = send(user.getFd(), fullMessage.c_str(), fullMessage.size(), 0);
    if (bytes < 0)
    {
        std::cerr << "Error sending message to client" << std::endl;
    }
}

bool Server::requireRegistered(User & user)
{
    if (!user.isRegistered())
    {
        sendToClient(user, ":ircserv 451 " + getClientName(user) + " :You have not registered yet");
        return false;
    }
    return true;
}