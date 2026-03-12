
#include "../include/Helpers.hpp"
#include "../include/Server.hpp"
#include "../include/User.hpp"
#include "../include/Message.hpp"


#include <string>
#include <exception>
#include <errno.h>
#include <unistd.h>//close
#include <sys/socket.h>//accept recv
#include <iostream>

// int main(int ac, char **av) {
//     try {
//         check_arg(ac, av);
//         signal(SIGINT, sigStopHandler); // ctrl + c
//         signal(SIGQUIT, sigStopHandler);// ctrl + backlash 

//         Server server(std::atoi(av[1]), av[2]);
//         server.run();
//     }
//     catch(const std::exception &e) {
//         std::cerr << RED "!! ERROR !! " << e.what() << RESET << std::endl;
//         return 1;
//     }
//     return 0;
// }



int main()
{
    Server server(6667, "pass1234");
    User user(42);

    std::cout << "TEST OK" << std::endl;
    Message msg1;
    msg1._command = "USER";
    msg1._params.push_back("sara");
    msg1._params.push_back("0");
    msg1._params.push_back("*");
    msg1._trailing = "Sara Bellili";

    server.dispatchCommand(user, msg1);

    std::cout << "username after USER = " << user.getUsername() << std::endl;
    std::cout << std::endl;

    std::cout << "TEST USER DEJA SET" << std::endl;
    Message msg2;
    msg2._command = "USER";
    msg2._params.push_back("autreuser");
    msg2._params.push_back("0");
    msg2._params.push_back("*");
    msg2._trailing = "Autre Nom";

    server.dispatchCommand(user, msg2);

    std::cout << std::endl;

    std::cout << "TEST manque arg" << std::endl;
    User user2(43);
    Message msg3;
    msg3._command = "USER";
    msg3._params.push_back("bob");
    msg3._params.push_back("0");

    server.dispatchCommand(user2, msg3);

    return 0;
}


void Server::run()
{
    std::cout << "Server running !\nPort: " << this->_port << "\nPassword: " << this->_password<< std::endl;
    //Creation de la sociket d'ecoute
        //init de la socket = creer la 'prise' reseau _serverFd = socket(.......) ??
        //bind() = donner un port un serveur (association de la socket a une UP et un port)
        //listen() = en mode attente d'appel
        //une fonction pour pas que la socket soit non bloquante, a trouver dnas la liste des fonctions proposees, si rien n'est dispo
        //le moment on on construit le serveur : _running 
    while(_running)
    {
        poll(&_pollFds[0], _pollFds.size(), -1);//timeout a revoir?
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
