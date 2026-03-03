
#include "../include/Helpers.hpp"
#include "../include/Server.hpp"
#include "../include/User.hpp"
#include "../include/Message.hpp"

#include <csignal>
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>
#include <errno.h>
#include <unistd.h>//close
#include <sys/socket.h>//accept recv
#include <iostream>

void    sigStopHandler(int signum) {
    std::cout << "Interruption du serveur par signal " << signum << std::endl;
    std::exit(signum); // exit pas dans les fonctions allowed, TODO: une sorte de ft clean close
}

void    check_arg(int ac, char **av) {
        if (ac != 3)
            throw std::logic_error("Wrong nbr of arguments. Use ./irc <port> <password>\n");
        for (int i=0; i<ac; i++) {
            std::cout << "av[" << i << "] : " << av[i] << std::endl;
        }
        //TODO: 
        //pour verif port, est-ce qu'une fonction existe pour test ?
        //option check secure password (bif bof la motiv)
}

int main(int ac, char **av) {
    try {
        check_arg(ac, av);
        signal(SIGINT, sigStopHandler); // ctrl + c
        signal(SIGQUIT, sigStopHandler);// ctrl + backlash 
        int port = ft_atoi_port(av[1]);
        std::string password = av[2];
        Server server(port, password);
        server.run();
    }
    catch(const std::logic_error &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}

void Server::run()
{
    //Creation de la sociket d'ecoute
        //init de la socket = creer la 'prise' reseau _serverFd = socket(.......) ??
        //bind() = donner un port un serveur (association de la socket a une UP et un port)
        //listen() = en mode attente d'appel
        //une fonction pour pas que la socket soit non bloquante, a trouver dnas la liste des fonctions proposees, si rien n'est dispo
        //le moment on on construit le serveur : _running 
    while(_running)
    {
        poll(_pollFds.data(), _pollFds.size(), -1);//timeout a revoir?
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
            //IF p.fd != fd du serveur + POLLIN = on a un client existant qui tente d'interargir
                //Demarrage du parsing(p.fd) --> emporte tous les elements du client TO DO START OF PARSING
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
