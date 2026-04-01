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
#include <cerrno>

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
 
void    Server::disconnectClient(int clientFd)
{
    std::map<int, User*>::iterator userIt = _usersByFd.find(clientFd);
    if (userIt == _usersByFd.end())//verifie si le clientFd correspond bien a un user existant
        return;
    User* user = userIt->second;
    if (!user->getNick().empty())//verifie si le user a un nick avant de le supprimer. Empty test dans le cas d'une deconnection avant d'avoir choiss son nick
        _usersByNick.erase(user->getNick());
    for (std::map<std::string, Channel*>::iterator chanIt = _channels.begin(); chanIt != _channels.end(); )//on parcourt tous nos channels
    {
        Channel* channel = chanIt->second;//je recupere mon channel courant
        if (channel->hasUser(user))//retirer le user du channel
            channel->removeUser(user);
        if (channel->getUsers().empty())//retirer les objets channels vides
        {
            delete channel;
            std::map<std::string, Channel*>::iterator toErase = chanIt++;
            _channels.erase(toErase);
        }
        else//si pas vide je passe a la suite
        {
            ++chanIt;
        }
    }
    for (std::vector<pollfd>::iterator it = _pollFds.begin(); it != _pollFds.end(); ++it)//nettoyer notre vector pollfds
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
    std::cerr << YELLOW "--> Client/fd " << clientFd << " Disconnected !" RESET << std::endl;
}

/*
SARA --> a partager plus tard
On nettoie pas _usersByNick, les channels ou le users est present, le soeprateurs si le user etait op, les inve

Pointeurs morts dans _usersNick _channels _users _operators _invitedUsers
Logique : retrouver user* > enlever de _usersByNick > enlever tous les channels > enlever des operators si jamais ya besoin
> enlever des invites si besoin > supprimer les eventuels channels vides


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


void Server::initServerFd()
{

    struct addrinfo hints; 
    struct addrinfo * result; 

    memset(&hints, 0, sizeof(struct addrinfo));
    
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    hints.ai_flags = AI_PASSIVE; // adresse passive = adresse en ecoute donc adresse serveur

    if (getaddrinfo(NULL, _raw_port, &hints, &result) != 0 ) {
        throw std::logic_error("No port available. Cannot launch server. ");
    }

    _serverFd = socket(result->ai_family, result->ai_socktype, result->ai_protocol); 
    if (_serverFd < 0 ) {
        freeaddrinfo(result);
        throw std::logic_error("Fail socket. Cannot launch server. ");
    }

    /*Ajout d'un bloc verif fcntl ici comme dnas run ? */
    
    int yes = 1;// ci apres, ajout des eventuelles options a config sur la socket// liste des options de config socket sur ce lien : https://fr.manpages.org/socket/7
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
    if (listen (_serverFd, 10) < 0 )
    {
        close(_serverFd);
        _serverFd = -1;
        throw std::logic_error("Fail listen. Cannot launch server. ");
    }
}


void Server::run()
{
    std::cout << "Starting new server. \nPort: " << this->_port 
    << "\nPassword: " << this->_password << std::endl;
    g_run = 1; 
    initServerFd();
    std::cout << GREEN "SERVER LISTENING :" RESET << std::endl;
    pollfd pfd_server = {this->_serverFd, POLLIN, 0};
    _pollFds.push_back(pfd_server);

    while (g_run == 1)
    {   
        // std::cout << CYAN "Server on ? " << g_run << RESET << std::endl;
        // About Timeout : now a -1 pour rien bloquer, mais l'option d'en set un est importante, espace delais entrenouveaux appel de time out donc "eco ressources " ce sont les events de tentative de recennexion successive sur un server
        int pollReturn = poll(&_pollFds[0], _pollFds.size(), 2000);
        if (pollReturn < 0)
        {
            if (errno == EINTR)//cas ctrl c pour dire que c'est un arret normal e tpas un crash
                break;
            throw std::logic_error("poll() failed");

        }
        if (pollReturn == 0)
            continue;
        for (size_t i = 0; i < _pollFds.size(); ++i)
        {
            int fd = _pollFds[i].fd;//on avait une reference, je retire car _pollFds peut etre modifie quand on disconnect un client --> dangereux
            short revents = _pollFds[i].revents;
    
            if (revents == 0)
                continue; //TODO: pertinence de check meme si revents 0 ? => oui par securite
            if (revents & (POLLERR | POLLHUP | POLLNVAL)) {
                //TODO: gestion des erreurs et clean de fd
                // std::cerr << "message d'erreur" << std::endl;
                // couper la connexion+ remove fd + client
                /*
                POLLHUP client a ferme la co --> disconnectClient
                POLLER erreur sur la socket style probleme reseau == > fermer nettoyer 
                POLLNVAL fd invalide deja ferme, corrompu etc --> retirer nettoyer 
                */
                if (fd == _serverFd)
                    throw std::logic_error("server socket poll error");
                disconnectClient(fd);
                --i;
                continue;
            }
            //  NOUVELLE CONNEXION 
            if (fd == _serverFd && (revents & POLLIN))
            { //formulation bizarre mais en gros POLLIN and co sont des masques

                struct sockaddr_in client_addr; //TODO importance/utilite d'usage de la structure sockaddress

                socklen_t addr_size = sizeof(client_addr);
        
                int client_fd = accept(_serverFd, (struct sockaddr *)&client_addr, &addr_size);//J'accepte une nouvelle connexion entrante
                if (client_fd < 0)//si accept echoue
                {
                    if (errno == EAGAIN || errno == EWOULDBLOCK)//si l'echec vient de ya en fait aucune connexion prete mtn = ressaye + tard
                        continue;
                    throw std::logic_error("fail connexion client => accept() error "); //sinon si vraie erreur on stop le sevreur
                }
                    
                if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1)//on tente de mettre la socket du client en mode non bloquant 
                {
                    close(client_fd);
                    _serverFd = -1;
                    throw std::logic_error("cannot setup socket as nonblock "); 
                }

                struct pollfd newFdToPoll;
                newFdToPoll.fd = client_fd;
                newFdToPoll.events = POLLIN;
                newFdToPoll.revents = 0;

                _usersByFd[client_fd] = new User(client_fd); //recup des infos du client from sockaddress
                

                _pollFds.push_back(newFdToPoll);
                std::cout << "New client connected: fd " << client_fd << std::endl;
            }
            //  MESSAGE CLIENT
            else if (revents & POLLIN)
            {
                size_t oldSize = _pollFds.size();
                onClientRead(fd);
                if (_pollFds.size() < oldSize)
                    --i;
            }
        }
    }
    std::cout << YELLOW "server timeout" RESET << std::endl;
    for (size_t i = 0; i < _pollFds.size();)
    {
        int fd = _pollFds[i].fd;
        if (fd == _serverFd)
        {
            ++i;
            continue;
        }
        send(fd, "Serveur Closed. Bye bye !\n", 26, 0);
        disconnectClient(fd);
    }
    close(_serverFd);
    _pollFds.clear();

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


