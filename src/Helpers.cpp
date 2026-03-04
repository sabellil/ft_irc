#include "../include/Helpers.hpp"
#include <stdexcept>


void    check_arg(int ac, char **av) {
        if (ac != 3)
            throw std::logic_error("Wrong nbr of arguments. Use ./irc <port> <password>");

        int port_wanted = std::atoi(av[1]); //std::atoi return 0 si conversion impossible
        if (port_wanted <= 0 || port_wanted > 65535)
            throw std::logic_error("Invalid port: 1 =< port > 65535 ");

        std::string password = av[2];
        if (password.size() < 4)
            throw std::logic_error("Password : 4 char minimum required");

        for (int i = 0; i <= password.size(); i++) {
            if (!isgraph(password[i]))
                throw std::logic_error("Invalid Password : only letters, symbols or digit allowed");
        }
        std::cout << GREEN "ARG OK" RESET << std::endl;
        return;
        
//BROUILLON
        // if (password.find_first_of(password.ischar()))
        // for(std::allocator<char> it = password.begin(); ; i++)
//PSEUDOCODE
        //boucle for qui parcourt la string avec un iterator (c++ style)
        //si elle trouve un char !ischar 
        //trow exception
}

// TODO: All ports below 1024 are RESERVED (unless you're the superuser)! (source guide des sockets)
//      proprosition de restreindre les ports autorises
//TODO: de meme pour le mot de passe, 
//        est-ce qu'une mini-regle de 3 caracters max serait pas pertinente ?

void    sigStopHandler(int signum) {
    std::cerr << "Interruption du serveur par signal " << signum << std::endl;
    std::exit(signum); // exit pas dans les fonctions allowed, TODO: une sorte de ft clean close
}

