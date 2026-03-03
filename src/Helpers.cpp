#include "../include/Helpers.hpp"
#include <stdexcept>

// int ft_atoi_port(const char* str)
// {
//     long res = 0;
//     int i = 0;

//     if (!str || !str[0])
//         throw std::logic_error("Invalid port: empty\n");

//     while (str[i])
//     {
//         if (str[i] < '0' || str[i] > '9')
//             throw std::logic_error("Invalid port: digits only\n");

//         res = res * 10 + (str[i] - '0');

//         if (res > 65535)
//             throw std::logic_error("Invalid port: out fo range\n");

//         i++;
//     }

//     if (res < 1)
//         throw std::logic_error("Invalid port: must be >= 1\n");

//     return (int)res;
// }

void    check_arg(int ac, char **av) {
        if (ac != 3)
            throw std::logic_error("Wrong nbr of arguments. Use ./irc <port> <password>");

        int port_wanted = std::atoi(av[1]); //std::atoi return 0 si conversion impossible
        if (port_wanted <= 0 || port_wanted > 65535)
            throw std::logic_error("Invalid port: 1 =< port > 65535 ");

        std::string password = av[2];
        if (password.empty() || !password.find_first_of(' '))
            throw std::logic_error("Password can't be empty or with spaces");
}

void    sigStopHandler(int signum) {
    std::cerr << "Interruption du serveur par signal " << signum << std::endl;
    std::exit(signum); // exit pas dans les fonctions allowed, TODO: une sorte de ft clean close
}
