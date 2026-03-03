
#include "Helpers.hpp"

#include <csignal>
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// void    sigStopHandler(int signum) {
//     std::cout << "Interruption du serveur par signal " << signum << std::endl;
//     std::exit(signum); // exit pas dans les fonctions allowed, TODO: une sorte de ft clean close
// }

// void    check_arg(int ac, char **av) {
//         if (ac != 3)
//             throw std::logic_error("Wrong nbr of arguments. Use ./irc <port> <password>\n");
//         for (int i=0; i<ac; i++) {
//             std::cout << "av[" << i << "] : " << av[i] << std::endl;
//         }
//         //TODO: 
//         //pour verif port, est-ce qu'une fonction existe pour test ?
//         //option check secure password (bif bof la motiv)
// }

// int main(int ac, char **av) {
//     try {
//         check_arg(ac, av);
//         signal(SIGINT, *sigStopHandler); // ctrl + c
//         signal(SIGQUIT, *sigStopHandler);
//         int port = ft_atoi_port(av[1]);
//         std::string password = av[2];
//         Server server(port, password);
//         server.run();
//     }
//     catch {
//         //error
//     }
// }



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
