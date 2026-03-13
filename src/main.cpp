
#include "../include/Helpers.hpp"
#include "../include/Server.hpp"
#include "../include/User.hpp"
#include "../include/Message.hpp"


#include <string>
#include <exception>
#include <cstdlib>
#include <errno.h>
#include <unistd.h>//close
#include <sys/socket.h>//accept recv
#include <iostream>

#include <netdb.h>
#include <arpa/inet.h> //ai_family
#include <string.h> //memset

int main(int ac, char **av) {
    try {
        check_arg(ac, av);
        signal(SIGINT, sigStopHandler); // ctrl + c
        signal(SIGQUIT, sigStopHandler);// ctrl + backlash 

        Server server(av[1], av[2]);
        server.run();
    }
    catch(const std::exception &e) {
        std::cerr << RED "!! ERROR !! " << e.what() << RESET << std::endl;
        return 1;
    }
    return 0;
}

void Server::run()
{
    std::cout << "Starting new server. \nPort: " << this->_port 
    << "\nPassword: " << this->_password
    << "\nraw: " << this->_raw_port
    << std::endl;


    //Creation de la sociket d'ecoute
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
        _running = true;
        int sockfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol); // c'est ici qu'on gere l'unbind auto du port
        if (sockfd < 0 )
            throw std::logic_error("Fail socket. Cannot launch server. ");
    
        if (bind(sockfd, result->ai_addr, result->ai_addrlen) < 0 )
            throw std::logic_error("Fail bind. Cannot launch server. ");

        if (listen (sockfd, 10) < 0 )
            throw std::logic_error("deaf port. Cannot launch server. ");
        std::cout << "SERVER LISTENING \n";

        //TODO ajouter le fd server a pollfd 1
    while(_running)
    {
        // freeaddrinfo(result); // getaddrinfo alloue de la memoire ! ne pas oublier de la free
        
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
    
            int client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_size);
    
            if (client_fd < 0)
                throw std::logic_error("fail connexion client.. "); //= check si une connexion est possible, et que le serveur a bien recu le fd du client
            std::cout << GREEN "\nNew Client connected !!" RESET << std::endl;
            //IF p.fd != fd du serveur + POLLIN = on a un client existant qui tente d'interargir
                //Demarrage du parsing(p.fd) --> emporte tous les elements du client TO DO START OF PARSING
                // onClientRead(p.fd);
        }
    }
}

/*
Boucle while (1)
{
    Lancement serveur si parsing initial OK
    if (peut lire sur la socket) POLLIN
        read();
    if (peut ecrire sur la socket) POLLOUT
        write();
}
*/

/*
TODO: 

- Ecrire classe Serveur
- Ecrire boucle main de sondage du serveur (CANCELED)
    - timeout ?
- Lister commandes client et interpretation IRC
- Ecrire classe Message
*/
