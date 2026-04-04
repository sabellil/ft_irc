#include "../include/Server.hpp"
#include "../include/User.hpp"
#include "../include/Message.hpp"
#include "../include/Channel.hpp"

#include <set>
#include <iostream>
#include <sys/socket.h>
#include <cstdlib>


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
    else if (cmd == "PING")
        handlePING(user, msg);
    else if (cmd == "PART")
        handlePART(user, msg);
    else
        handleUnknown(user, msg);
}

void Server::handlePING(User& user, const Message& msg)//on garde la connexion vivante
{
    if (!msg._trailing.empty())
        sendToClient(user, ":ircserv PONG :" + msg._trailing);
    else if (!msg._params.empty())
        sendToClient(user, ":ircserv PONG :" + msg._params[0]);
    else
        sendToClient(user, ":ircserv PONG :ircserv");
}

std::string Server::getClientName(const User& user) const
{
    if (user.getNick().empty())
    {
        return "*";
    }
    return user.getNick();
}

// void Server::sendToClient(User& user, const std::string& message)
// {
//     std::string fullMessage = message + "\r\n";
//     ssize_t bytes = send(user.getFd(), fullMessage.c_str(), fullMessage.size(), 0);
//     if (bytes < 0)
//     {
//         std::cerr << "Error sending message to client" << std::endl;
//     }
// }



bool Server::requireRegistered(User & user)
{
    if (!user.isRegistered())
    {
        sendToClient(user, ":ircserv 451 " + getClientName(user) + " :You have not registered yet");
        return false;
    }
    return true;
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
        user.setShouldDisconnect(true);
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
    std::string oldNick = user.getNick();
    if (!oldNick.empty())
        _usersByNick.erase(oldNick);

    user.setNick(newNick);
    user.setHasNick(true);
    _usersByNick[newNick] = &user;//enregistrer que ce nouveau nickname appartient a cet user la
    if (!oldNick.empty())
    {
        std::string nickMsg = ":" + oldNick + "!" + user.getUsername() + "@localhost NICK :" + newNick;
        sendToClient(user, nickMsg);
    }
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
    std::cout << "User registered:" << user.getNick() << " ("<< user.getUsername() <<")" << std::endl;
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

    if (channel->hasUserLimit() && channel->getUsers().size() >= (size_t)channel->getUserLimit())
    {
        sendToClient(user, ":ircserv 471 " + user.getNick() + " " + channelName + " :Channel is full");
        return;
    }
    if (!isNewChannel && channel->isInviteOnly() && !channel->isInvited(&user))
    {
        sendToClient(user, ":ircserv 473 " + user.getNick() + " " + channelName + " :Cannot join channel (+i)");
        return;
    }
    if (!isNewChannel && channel->hasKey())
    {
        if (msg._params.size() < 2 || msg._params[1] != channel->getKey())
        {
            sendToClient(user, ":ircserv 475 " + user.getNick() + " " + channelName + " :Cannot join channel (+k)");
            return;
        }
    }
 

    channel->addUser(&user);
    channel->removeInvite(&user);

    if (isNewChannel)
        channel->addOperator(&user);

    std::string joinMsg = ":" + user.getNick() + "!" + user.getUsername() + "@localhost JOIN " + channelName;

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
            names += "@";
        names += (*it)->getNick();
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
    for (std::map<std::string, User*>::iterator it = _usersByNick.begin(); it != _usersByNick.end(); ++it)
        std::cout << "[" << it->first << "]" << std::endl;
    if (_usersByNick.count(target))
    {
        User* targetUser = _usersByNick[target];
        std::cout << "PRIVMSG to nick = [" << target << "]" << std::endl;
        std::cout << "fullMsg = [" << fullMsg << "]" << std::endl;
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
    sendToClient(user, ":ircserv 401 " + user.getNick() + " " + target + " :No such nick/channel");
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

    if (msg._params.size() < 2)
    {
        sendToClient(user, ":ircserv 461 " + user.getNick() + " KICK :Not enough parameters");
        return;
    }

    const std::string& channelName = msg._params[0];
    const std::string& targetNick = msg._params[1];

    if (_channels.count(channelName) == 0)
    {
        sendToClient(user, ":ircserv 403 " + user.getNick() + " " + channelName + " :No such channel");
        return;
    }

    Channel* channel = _channels[channelName];

    if (!channel->hasUser(&user))
    {
        sendToClient(user, ":ircserv 442 " + user.getNick() + " " + channelName + " :You're not on that channel");
        return;
    }

    if (!channel->isOperator(&user))
    {
        sendToClient(user, ":ircserv 482 " + user.getNick() + " " + channelName + " :You're not channel operator");
        return;
    }


    if (_usersByNick.count(targetNick) == 0)
    {
        sendToClient(user, ":ircserv 401 " + user.getNick() + " " + targetNick + " :No such nick");
        return;
    }

    User* targetUser = _usersByNick[targetNick];

    if (!channel->hasUser(targetUser))
    {
        sendToClient(user, ":ircserv 441 " + user.getNick() + " " + targetNick + " " + channelName + " :They aren't on that channel");
        return;
    }

    std::string kickMsg = ":" + user.getNick() + "!" + user.getUsername() + "@localhost KICK " + channelName + " " + targetNick;
    if (!msg._trailing.empty())
        kickMsg += " :" + msg._trailing;
    const std::set<User*>& users = channel->getUsers();
    for (std::set<User*>::const_iterator it = users.begin(); it != users.end(); ++it)
    {
        sendToClient(**it, kickMsg);
    }
    channel->removeUser(targetUser);
    if (channel->getUsers().empty())
    {
        delete channel;
        _channels.erase(channelName);
    }
}

//INVITE <nick> <channel>
void Server::handleINVITE(User& user, const Message& msg)
{
    if (!requireRegistered(user))
        return;
    if (msg._params.size() < 2)
    {
        sendToClient(user, ":ircserv 461 " + user.getNick() + " INVITE :Not enough parameters");
        return;
    }

    const std::string& targetNick = msg._params[0];
    const std::string& channelName = msg._params[1];

    if (_channels.count(channelName) == 0)
    {
        sendToClient(user, ":ircserv 403 " + user.getNick() + " " + channelName + " :No such channel");
        return;
    }

    Channel* channel = _channels[channelName];

    if (!channel->hasUser(&user))
    {
        sendToClient(user, ":ircserv 442 " + user.getNick() + " " + channelName + " :You're not on that channel");
        return;
    }
    
    if (!channel->isOperator(&user))
    {
        sendToClient(user, ":ircserv 482 " + user.getNick() + " " + channelName + " :You're not channel operator");
        return;
    }

    if (_usersByNick.count(targetNick) == 0)
    {
        sendToClient(user, ":ircserv 401 " + user.getNick() + " " + targetNick + " :No such nick");
        return;
    }
    User* targetUser = _usersByNick[targetNick];

    if (channel->hasUser(targetUser))
    {
        sendToClient(user, ":ircserv 443 " + user.getNick() + " " + targetNick + " " + channelName + " :is already on channel");
        return;
    }
    channel->addInvited(targetUser);
    sendToClient(user, ":ircsev 341 " + user.getNick() + " " + targetNick + " " + channelName);
    sendToClient(*targetUser, ":" + user.getNick() + "!" + user.getUsername() + "@localhost INVITE " + targetNick + " :" + channelName);

}
/*
Verifs :
- assez de params
- channel existe
- le user qui invite est sur le channel
- si il est operetor si on est mode invite only
- la cible existe
- la cible n'est pas deja dans le channel
- ajouter la cible aux invites du channel
- envoyer la reponse a l'inviteur
- envoyer le msg INVITE a la target
*/


void Server::handleTOPIC(User& user, const Message& msg)
{
    if (!requireRegistered(user))
        return;
    
    if (msg._params.empty())
    {
        sendToClient(user, ":ircserv 461 " + user.getNick() + " TOPIC :Not enough parameters");
        return;
    }

    const std::string& channelName = msg._params[0];

    if (_channels.count(channelName) == 0)
    {
        sendToClient(user, ":ircserv 403 " + user.getNick() + " " + channelName + " :No such channel");
        return;
    }
    
    Channel* channel = _channels[channelName];

    if (!channel->hasUser(&user))
    {
        sendToClient(user, ":ircserv 442 " + user.getNick() + " " + channelName + " :You're not on that channel");
        return;
    }
    if (msg._trailing.empty())
    {
        if (channel->getTopic().empty())
        {
            sendToClient(user, ":ircserv 331 " + user.getNick() + " " + channelName + " :No topic is set");
        }
        else
        {
            sendToClient(user, ":ircserv 332 " + user.getNick() + " " + channelName + " :" + channel->getTopic());
        }
        return;
    }
    if (channel->isTopicRestricted() && !channel->isOperator(&user))
    {
        sendToClient(user, ":ircserv 482 " + user.getNick() + " " + channelName + " :You're not channel operator");
        return;
    }
    channel->setTopic(msg._trailing);
    std::string topicMsg = ":" + user.getNick() + "!" + user.getUsername() + "@localhost TOPIC " + channelName + " :" + msg._trailing;
    const std::set<User*>& users = channel->getUsers();
    for (std::set<User*>::const_iterator it = users.begin(); it != users.end(); ++it)
    {
        sendToClient(**it, topicMsg);
    }
}

/*
Utilisations:
- TOPIC #chan -->lire le topic
- TOPIC #chan :nouveau topic --> update le topic
- user enregistrer
- au moisn un param
- channel existant
- si pas de trailing j'affiche el topic actuel
- si trailing present je dois changer le topic
- si channel->istopicrestricted et user pas op --> nope
- sinon channel->setTopic(msg._trailing)
- affichage du nouveau topic a tous les membres du channel 
*/


void Server::handleMODE(User& user, const Message& msg)
{
    if (!requireRegistered(user))
        return;
    if (msg._params.size() < 2)
    {
        sendToClient(user, ":ircserv 461 " + user.getNick() + " MODE :Not enough parameters");
        return;
    }
    const std::string& channelName = msg._params[0];
    const std::string& modeString = msg._params[1];
    if (modeString.size() != 2)
    {
        sendToClient(user, ":ircserv 472 " + user.getNick() + " :Only one mode at a time");
        return;
    }
    if (_channels.count(channelName) == 0)
    {
        sendToClient(user, ":ircserv 403 " + user.getNick() + " " + channelName + " :No such channel");
        return;
    }
    Channel* channel = _channels[channelName];
    if (!channel->hasUser(&user))
    {
        sendToClient(user, ":ircserv 442 " + user.getNick() + " " + channelName + " :You're not on that channel");
        return;
    }
    if (!channel->isOperator(&user))
    {
        sendToClient(user, ":ircserv 482 " + user.getNick() + " " + channelName + " :You're not channel operator");
        return;
    }
    if (modeString.size() < 2 || (modeString[0] != '+' && modeString[0] != '-'))
    {
        std::string badMode(1, modeString[1]);
        sendToClient(user, ":ircserv 472 " + user.getNick() + " " + badMode + " :is unknown mode char to me");
        return;
    }
    char sign = modeString[0];
    char mode = modeString[1];

    if (mode == 'i')
    {
        if (sign == '+')
            channel->setInviteOnly(true);
        else
            channel->setInviteOnly(false);
    }
    else if (mode == 't')
    {
        if (sign == '+')
            channel->setTopicRestricted(true);
        else
            channel->setTopicRestricted(false);
    }
    else if (mode == 'k')
    {
        if (sign == '+')
        {
            if (msg._params.size() < 3)
            {
                sendToClient(user, ":ircserv 461 " + user.getNick() + " MODE :Not enough parameters");
                return;
            }
            channel->setKey(msg._params[2]);
        }
        else
        {
            channel->removeKey();
        }
    }
    else if (mode == 'l')
    {
        if (sign == '+')
        {
            if (msg._params.size() < 3)
            {
                sendToClient(user, ":ircserv 461 " + user.getNick() + " MODE :Not enough parameters");
                return;
            }
            int limit = atoi(msg._params[2].c_str());
            if (limit <= 0)
            {
                sendToClient(user, ":ircserv 696 " + user.getNick() + " :Invalid limit");
                return;
            }
            channel->setUserLimit(limit);
        }
        else
        {
            channel->removeUserLimit();
        }
    }
    else if (mode == 'o')
    {
        if (msg._params.size() < 3)
        {
            sendToClient(user, ":ircserv 461 " + user.getNick() + " MODE :Not enough parameters");
            return;
        }
        const std::string& targetNick = msg._params[2];
        if (_usersByNick.count(targetNick) == 0)
        {
            sendToClient(user, ":ircserv 401 " + user.getNick() + " " + targetNick + " :No such nick");
            return;
        }
        User* targetUser = _usersByNick[targetNick];
        if (!channel->hasUser(targetUser))
        {
            sendToClient(user, ":ircserv 441 " + user.getNick() + " " + targetNick + " " + channelName + " :They aren't on that channel");
            return;
        }
        if (sign == '+')
            channel->addOperator(targetUser);
        else
            channel->removeOperator(targetUser);
    }
    else
    {
        std::string badMode(1, modeString[1]);
        sendToClient(user, ":ircserv 472 " + user.getNick() + " " + badMode + " :is unknown mode char to me");
        return;
    }

    std::string modeMsg = ":" + user.getNick() + "!" + user.getUsername() + "@localhost MODE " + channelName + " " + modeString;
    if (mode == 'k' && sign == '+')
        modeMsg += " " + msg._params[2];
    if (mode == 'l' && sign == '+')
        modeMsg += " " + msg._params[2];
    if (mode == 'o')
        modeMsg += " " + msg._params[2];
    const std::set<User*>& users = channel->getUsers();
    for (std::set<User*>::const_iterator it = users.begin(); it != users.end(); ++it)
    {
        sendToClient(**it, modeMsg);
    }
}
    /*
    - verif si param < 3
    - verif si nick existe dans _usersByNick
    - recuperer ma targetUser
    - verif si targetUser est bien dans le channel
    - si +o je addOperator(targetUser)
    - si -o je remove
    
    */

/*
TO DO:
- i --> mettre ou enlever du mode invite only un channel
- t --> mettre ou enlever droit de la commande TOPIC pour operators
- k --> mettre ou enlever mdp pour le channel
- l --> donner ou enlever droit operator a un user
- o --> mettre ou enlever nb de user limit par channel
    MODE #chan +o nick
- Update handleJOIN suite a handleMODE -t
- Update handleINVITE suite a handleMODE -i
- Update disconnectClient

*/

void Server::handlePART(User& user, const Message& msg)
{
    std::cout << "DEBUG: handlePART called" << std::endl;

    if (!requireRegistered(user))
        return;

    if (msg._params.empty())
    {
        sendToClient(user, ":ircserv 461 " + user.getNick() + " PART :Not enough parameters");
        return;
    }

    const std::string& channelName = msg._params[0];

    if (_channels.count(channelName) == 0)
    {
        sendToClient(user, ":ircserv 403 " + user.getNick() + " " + channelName + " :No such channel");
        return;
    }

    Channel* channel = _channels[channelName];

    if(!channel->hasUser(&user))
    {
        sendToClient(user, ":ircserv 442 " + user.getNick() + " " + channelName + " :You're not on that channel");
        return;
    }
    std::string partMsg = ":" + user.getNick() + "!" + user.getUsername() + "@localhost PART " + channelName;
    if (!msg._trailing.empty())
        partMsg += " :" + msg._trailing;
    const std::set<User*>& users = channel->getUsers();
    for (std::set<User*>::const_iterator it = users.begin(); it != users.end(); ++it)
        sendToClient(**it, partMsg);
    channel->removeUser(&user);
    if(channel->getUsers().empty())
    {
        delete channel;
        _channels.erase(channelName);
    }
}

void Server::handleUnknown(User& user, const Message& msg)
{
    sendToClient(user, ":ircserv 421 " + getClientName(user) + " " + msg._command + " :Unknown command");
}




