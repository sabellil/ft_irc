#include "../include/Helpers.hpp"
#include "../include/Server.hpp"
#include "../include/User.hpp"
#include "../include/Message.hpp"

#include <stdexcept>


void    check_arg(int ac, char **av) {
        if (ac != 3)
            throw std::logic_error("Wrong nbr of arguments. Use ./ircserv <port> <password>");

        int port_wanted = std::atoi(av[1]); //std::atoi return 0 si conversion impossible
        if (port_wanted <= 0 || port_wanted > 65535)
            throw std::logic_error("Invalid port: 1 =< port > 65535 ");

        std::string password = av[2];
        if (password.size() < 4)
            throw std::logic_error("Password : 4 char minimum required");

        for (unsigned long i = 0; i < password.size(); i++) {
            if (isgraph(password[i]) == false)
                throw std::logic_error("Invalid Password : only letters, symbols or digits allowed");
        }
        std::cout << GREEN "ARG OK" RESET << std::endl;
        return;
}

void    sigStopHandler(int signum) {
    
    (void)signum;
    std::cerr << BLUE " \r" RESET ; 
    // \r == le curseur revient au tout debut de la ligne (meme si des trucs sont deja ecrits, cheh le chapo C)
    g_run = 0;
    return;

}
