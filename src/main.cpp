
#include "Helpers.hpp"

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
        signal(SIGQUIT, sigStopHandler);
        int port = ft_atoi_port(av[1]);
        std::string password = av[2];
        Server server(port, password);
        server.run();
    }
    catch {
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
    while(_running)
    {
        poll(_pollFds.data(), _pollFds.size(), -1);//timeout a revoir?
        for (size_t i = 0; i < _pollFds.size(); ++i)//parocurir tous les descripteurs surveilles par poll
        {
            //si aucun event sur ce fd --> next one
            //si erreur sur ce fd= connexion cassee (POLLERR POLLHUP POLLNVAL) https://man7.org/linux/man-pages/man2/poll.2.html
                //fermer la connexion
                //supprimer le client de l structure _usersbyFd
                //supprimer le fd de _pollFds
        }
        //analyse des events
        //acceptation des nouveaux clients
        //lire les clients existants
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


int main(int ac, char **av)
{
    if (ac != 2)
    {
        std::cout << "Usage: ./test <port>\n";
        return 1;
    }

    try
    {
        int port = ft_atoi_port(av[1]);
        std::cout << "Valid port: " << port << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << "Error: " << e.what();
    }

    return 0;
}


/*
TODO: 

- Ecrire classe Serveur
- Ecrire boucle main de sondage du serveur (CANCELED)
    - timeout ?
- Lister commandes client et interpretation IRC
- Ecrire classe Message
*/
