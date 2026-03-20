#include "../include/Server.hpp"
#include "../include/User.hpp"
#include "../include/Message.hpp"
#include "../include/Errors.hpp"
#include "../include/colors.hpp"
#include "../include/Channel.hpp"

#include <set>
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

    int bytesRead = recv(clientFd, buffer, sizeof(buffer), 0);
    if (bytesRead == 0)//le client est deconnecte
    {
        disconnectClient(clientFd);
        std::cout << "Client disconnected" << std::endl;
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
    
    //DEBUG
    std::cout << "Client " << clientFd << ": " << buffer;
    send(clientFd, "PONG\n", 5, 0);
    // TODO:le buffer se clean pas entre plusieurs clients
    //DEBUG

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
        if (!msg.parse(line))
            continue;

        dispatchCommand(user, msg);
    }
}



//DEBUG
//std::cerr << "Checkpoint x" << std::endl;


void Server::initServerFd() {

    struct addrinfo hints; 
    struct addrinfo * result; 

    memset(&hints, 0, sizeof(struct addrinfo));
    
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    hints.ai_flags = AI_PASSIVE; // adresse passive = adresse en ecoute donc adresse serveur

    if (getaddrinfo(NULL, _raw_port, &hints, &result) != 0 ) {
        freeaddrinfo(result);
        throw std::logic_error("No port available. Cannot launch server. ");
    }

    _serverFd = socket(result->ai_family, result->ai_socktype, result->ai_protocol); 
    if (_serverFd < 0 ) {
        freeaddrinfo(result);
        throw std::logic_error("Fail socket. Cannot launch server. ");
    }
    
    int yes = 1;
    // ci apres, ajout des eventuelles options a config sur la socket
    // liste des options de config socket sur ce lien : https://fr.manpages.org/socket/7
    setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)); //ok re-use port sur deux serveurs lances successivements

    if (bind(_serverFd, result->ai_addr, result->ai_addrlen) < 0 ) {
        freeaddrinfo(result);
        throw std::logic_error("Fail bind. Cannot launch server. ");
    }
    freeaddrinfo(result);
    if (listen (_serverFd, 10) < 0 )
        throw std::logic_error("deaf port. Cannot launch server. ");
}


void Server::run()
{
    std::cout << "Starting new server. \nPort: " << this->_port 
    << "\nPassword: " << this->_password
    << std::endl;

    initServerFd();
    _running = true; 
    std::cout << GREEN "SERVER LISTENING :" RESET << std::endl;
    
    //probablement en faire une fct ou un constructeur doit exister
    pollfd pfd_server = {_serverFd, POLLIN, 0};
    _pollFds.push_back(pfd_server);

    while (_running)
    {
        poll(&_pollFds[0], _pollFds.size(), -1); // TODO: gestion d'erreur
        // About Timeout : now a -1 pour rien bloquer, mais l'option d'en set un est importante, espace delais entrenouveaux appel de time out donc "eco ressources " ce sont les events de tentative de recennexion successive sur un server

        for (size_t i = 0; i < _pollFds.size(); ++i)
        {
            pollfd &p = _pollFds[i];
            if (p.revents == 0)
                continue; //TODO: pertinence de check meme si revents 0 ?
            if (p.revents & (POLLERR | POLLHUP | POLLNVAL)) {
                //TODO: gestion des erreurs et clean de fd
                // std::cerr << "message d'erreur" << std::endl;
                // couper la connexion+ remove fd + client
                continue;
            }
            //  NOUVELLE CONNEXION 
            if (p.fd == _serverFd && (p.revents & POLLIN)) { //formulation bizarre mais en gros POLLIN and co sont des masques
                struct sockaddr_storage client_addr; //TODO importance/utilite d'usage de la structure sockaddress
                socklen_t addr_size = sizeof(client_addr);
        
                int client_fd = accept(_serverFd, (struct sockaddr *)&client_addr, &addr_size);
        
                if (client_fd < 0)
                    throw std::logic_error("fail connexion client.. "); 
                //= check si une connexion est possible, et que le serveur a bien recu le fd du client
                        // créer client TODO: creation/inti de l'objet client
                _usersByFd[client_fd] = new User(client_fd);
                pollfd pfd_client = {client_fd, POLLIN, 0}; //ajout du fd client a la boucle
                _pollFds.push_back(pfd_client);
                std::cout << "New client connected: fd " << client_fd << std::endl;
            }
            //  MESSAGE CLIENT
            else if (p.revents & POLLIN)
            {
                onClientRead(p.fd);
            }
        }
    }
}

    // while(_running)
    // {
        
        // poll(&_pollFds[0], _pollFds.size(), -1); //-1 = pas de timeout
        // std::cout << GREEN "\npoll ok !!" RESET << std::endl;
        // for (size_t i = 0; i < _pollFds.size(); ++i)//parocurir tous les descripteurs surveilles par poll
        // {
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

            //IF p.fd != fd du serveur + POLLIN = on a un client existant qui tente d'interargir
                //Demarrage du parsing(p.fd) --> emporte tous les elements du client TO DO START OF PARSING
                // onClientRead(p.fd);
//         }
//     }
// }


