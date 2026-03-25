#include "../include/Server.hpp"
#include "../include/User.hpp"
#include "../include/Message.hpp"
#include "../include/Errors.hpp"
#include "../include/colors.hpp"
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


Server::Server(int port, const std::string& password)
: _raw_port(NULL),
  _port(port),
  _password(password),
  _serverFd(-1)
{
}

Server::Server(char * raw_port, const std::string& password)
: _raw_port(raw_port),
  _port(std::atoi(raw_port)),
  _password(password),
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
        // std::cout << "Client disconnected" << std::endl;
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
    // std::cout << "\rClient " << clientFd << ": " << buffer ;
    // send(clientFd, "PONG\n", 5, 0);
    // TODO:le buffer se clean pas entre plusieurs clients
    //DEBUG

    User* user = _usersByFd[clientFd];
    user->inbuf().append(buffer, bytesRead);
    processInputBuffer(*user);
}
 
void    Server::disconnectClient(int clientFd) {

        //vire le fd du pollFd
        for (std::vector<pollfd>::iterator it = _pollFds.begin(); it != _pollFds.end(); ++it) {

            if ( it -> fd == clientFd) {
                _pollFds.erase(it);

                delete _usersByFd[clientFd];       
                _usersByFd.erase(clientFd); // petit coup de menage

                close(clientFd);
                std::cerr << YELLOW "--> Client/fd " << clientFd << " Disconnected !" RESET << std::endl;
                return;
            }
        }
        
}

/*
SARA --> a partager plus tard
On nettoie pas _userByNick, les channels ou le users est present, le soeprateurs si le user etait op, les inve
*/

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

    g_run = 1; 
    initServerFd();
    std::cout << GREEN "SERVER LISTENING :" RESET << std::endl;
    
    //    struct pollfd {
    //        int   fd;         /* file descriptor */
    //        short events;     /* requested events */
    //        short revents;    /* returned events */
    //    };
    // MEMO : pollfd est une structure C, pas un objet CPP
    pollfd pfd_server = {this->_serverFd, POLLIN, 0};
    _pollFds.push_back(pfd_server);

    while (g_run == 1)
    {
        poll(&_pollFds[0], _pollFds.size(), 2000); // TODO: gestion d'erreur
        // About Timeout : now a -1 pour rien bloquer, mais l'option d'en set un est importante, espace delais entrenouveaux appel de time out donc "eco ressources " ce sont les events de tentative de recennexion successive sur un server
        std::cout << CYAN "Server on ? " << g_run << RESET << std::endl;

        for (size_t i = 0; i < _pollFds.size(); ++i)
        {
            pollfd &p = _pollFds[i];
            if (p.revents == 0)
                continue; //TODO: pertinence de check meme si revents 0 ? => oui par securite
            if (p.revents & (POLLERR | POLLHUP | POLLNVAL)) {
                //TODO: gestion des erreurs et clean de fd
                // std::cerr << "message d'erreur" << std::endl;
                // couper la connexion+ remove fd + client
                continue;
            }
            //  NOUVELLE CONNEXION 
            if (p.fd == _serverFd && (p.revents & POLLIN)) { //formulation bizarre mais en gros POLLIN and co sont des masques
                
                struct sockaddr_in client_addr; //TODO importance/utilite d'usage de la structure sockaddress
                socklen_t addr_size = sizeof(client_addr);
                
                int client_fd = accept(_serverFd, (struct sockaddr *)&client_addr, &addr_size);
                if (client_fd < 0)
                    throw std::logic_error("fail connexion client => accept() error "); 
                    
                if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1)
                    throw std::logic_error("cannot setup socket as nonblock "); 

                struct pollfd newFdToPoll;
                newFdToPoll.fd = client_fd;
                newFdToPoll.events = POLLIN;
                newFdToPoll.revents = 0;

                _usersByFd[client_fd] = new User(client_fd); //recup des infos du client from sockaddress
                

                _pollFds.push_back(newFdToPoll);
                std::cout << "New client connected: fd " << client_fd << std::endl;
            }
            //  MESSAGE CLIENT
            else if (p.revents & POLLIN)
            {
                onClientRead(p.fd);
            }
        }
    }
    std::cout << YELLOW "server timeout" RESET << std::endl;
    for (std::vector<pollfd>::reverse_iterator it = _pollFds.rbegin(); it != _pollFds.rend(); ++it) { 
        if (it -> fd != 3) 
            send(it -> fd, "Serveur Closed. Bye bye !\n", 26, 0); 
        delete _usersByFd[it -> fd];       
        _usersByFd.erase(it -> fd); // petit coup de menage
        close(it -> fd);
        std::cerr << YELLOW "--> stopping server : Fd " << it -> fd << " Disconnected !" RESET << std::endl;
        _pollFds.pop_back();
    }

    std::cerr << RED "\rSERVER CLOSED" RESET << std::endl;
}
//post repas
//regarder les details de sockaddr_in et voir ce que je peux recup pour remplir mon user.


// /* Structure describing an Internet socket address.  */
// struct sockaddr_in
//   {
//     __SOCKADDR_COMMON (sin_);
//     in_port_t sin_port;			/* Port number.  */
//     struct in_addr sin_addr;		/* Internet address.  */

//     /* Pad to size of `struct sockaddr'.  */
//     unsigned char sin_zero[sizeof (struct sockaddr)
// 			   - __SOCKADDR_COMMON_SIZE
// 			   - sizeof (in_port_t)
// 			   - sizeof (struct in_addr)];
//   };

// #if !__USE_KERNEL_IPV6_DEFS
// /* Ditto, for IPv6.  */
// struct sockaddr_in6
//   {
//     __SOCKADDR_COMMON (sin6_);
//     in_port_t sin6_port;	/* Transport layer port # */
//     uint32_t sin6_flowinfo;	/* IPv6 flow information */
//     struct in6_addr sin6_addr;	/* IPv6 address */
//     uint32_t sin6_scope_id;	/* IPv6 scope-id */
//   };
// #endif /* !__USE_KERNEL_IPV6_DEFS */

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


