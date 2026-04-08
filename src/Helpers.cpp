#include "../include/Helpers.hpp"
#include "../include/Server.hpp"
#include "../include/User.hpp"
#include "../include/Message.hpp"

#include <stdexcept>


void    check_arg(int ac, char **av)
{
        if (ac != 3)
            throw std::logic_error("Wrong nbr of arguments. Use ./ircserv <port> <password>");
        
        std::string portStr = av[1];
        if (portStr.empty())
            throw std::logic_error("Invalid port : musn't be empty");
        for (size_t i = 0; i < portStr.size(); i++)
        {
            if (!std::isdigit(portStr[i]))
                throw std::logic_error("Invalid port : must contain only digits");
        }

        int port_wanted = std::atoi(av[1]);
        if (port_wanted < 1024 || port_wanted > 65535)
            throw std::logic_error("Invalid port: < port > must be between 1024 and 65535");

        std::string password = av[2];
        if (password.size() < 4)
            throw std::logic_error("Password : 4 char minimum required");

        for (unsigned long i = 0; i < password.size(); i++) {
            if (isgraph(password[i]) == false)
                throw std::logic_error("Invalid Password : only letters, symbols or digits allowed");
        }
        return;
}

void    sigStopHandler(int signum) {
    
    std::cerr << YELLOW "\rServer interrupted by signal " << signum << RESET << std::endl; 
    g_run = 0;
    return;

}

