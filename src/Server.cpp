#include "../include/Server.hpp"
#include "../include/User.hpp"
#include "../include/Message.hpp"
#include "../include/colors.hpp"

#include <iostream>
#include <cstdlib>
#include <cstring> //memset
#include <sys/socket.h>//accept recv
#include <sys/types.h>//
#include <netdb.h>
#include <arpa/inet.h> //ai_family


Server::Server(int port, const std::string& password)
: _raw_port(NULL),
  _port(port),
  _password(password),
  _running(true),
  _serverFd(-1)
{
}

Server::Server(char * raw_port, const std::string& password)
: _raw_port(raw_port),
  _port(std::atoi(raw_port)),
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
    (void)user;
    (void)msg;
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

//DEBUG
//std::cerr << "Checkpoint x" << std::endl;

void Server::initAndBindServerFd() {

    struct addrinfo hints; //le cahier des charges de l'adresse demandee
    struct addrinfo * result; //pointeur sur une liste chainee d'adresses possibles (svt on 8use la premiere)

    // init l'adresse a 0
    memset(&hints, 0, sizeof(struct addrinfo));
    
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    hints.ai_flags = AI_PASSIVE; // adresse passive = adresse en ecoute donc adresse serveur

    if (getaddrinfo(NULL, _raw_port, &hints, &result) != 0 )
        throw std::logic_error("No port available. Cannot launch server. ");

    _serverFd = socket(result->ai_family, result->ai_socktype, result->ai_protocol); // c'est ici qu'on gere l'unbind auto du port
    if (_serverFd < 0 )
        throw std::logic_error("Fail socket. Cannot launch server. ");
    // setsockopt(_serverfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)); //ligne qui permet de réattributionle port très rapidement

    if (bind(_serverFd, result->ai_addr, result->ai_addrlen) < 0 )
        throw std::logic_error("Fail bind. Cannot launch server. ");
    freeaddrinfo(result); // getaddrinfo alloue de la memoire ! ne pas oublier de la free
}


void Server::run()
{
    std::cout << "Starting new server. \nPort: " << this->_port 
    << "\nPassword: " << this->_password
    << "\nraw: " << this->_raw_port
    << std::endl;

    initAndBindServerFd();
        
    if (listen (_serverFd, 10) < 0 )
        throw std::logic_error("deaf port. Cannot launch server. ");
    _running = true; 
    std::cout << GREEN "SERVER LISTENING \n" RESET << std::endl;

    //TODO ajouter le fd server a pollfd 1
    while(_running)
    {
        
        poll(&_pollFds[0], _pollFds.size(), -1);//timeout a revoir? 
        //TODO: add un timeout valide (ca evite que les blocages bloquent infiniment) 2
        //grosso modo poll fd c'est la liste des fd surveilles par la ft poll
        std::cout << GREEN "\npoll ok !!" RESET << std::endl;
        for (size_t i = 0; i < _pollFds.size(); ++i)//parocurir tous les descripteurs surveilles par poll
        {
            //IF aucun event sur ce fd --> next one 
            //IF erreur sur ce fd= connexion cassee (POLLERR POLLHUP POLLNVAL) https://man7.org/linux/man-pages/man2/poll.2.html
                //fermer la connexion 
                //supprimer le client de l structure _usersbyFd
                //supprimer le fd de _pollFds
            //IF p.fd est fd du serveur + POLLIN = un nouveau client essaye de se connecter
                //accept() --> renvoie le nouveua fd http://manpagesfr.free.fr/man/man2/accept.2.html
                //remplir la structure User pour ce client
                //ajouter son fd a pollFds
                //on est sur le fd du serveur, on relance un coup ce process pour check si de nouveaux clients sont la
            struct sockaddr_storage client_addr;
            socklen_t addr_size = sizeof(client_addr);
    
            int client_fd = accept(_serverFd, (struct sockaddr *)&client_addr, &addr_size);
    
            if (client_fd < 0)
                throw std::logic_error("fail connexion client.. "); //= check si une connexion est possible, et que le serveur a bien recu le fd du client
            std::cout << GREEN "\nNew Client connected !!" RESET << std::endl;
            //IF p.fd != fd du serveur + POLLIN = on a un client existant qui tente d'interargir
                //Demarrage du parsing(p.fd) --> emporte tous les elements du client TO DO START OF PARSING
                // onClientRead(p.fd);
        }
    }
}
