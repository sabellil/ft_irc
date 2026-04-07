#include "../include/Helpers.hpp"
#include "../include/Server.hpp"
#include "../include/User.hpp"
#include "../include/Message.hpp"


#include <string>
#include <exception>
#include <cstdlib>
#include <unistd.h>//close
#include <sys/socket.h>//accept recv
#include <iostream>
#include <netdb.h>
#include <arpa/inet.h> //ai_family
#include <string.h> //memset

volatile sig_atomic_t g_run = 0;

int main(int ac, char **av) {
    try {
        check_arg(ac, av);
        signal(SIGINT, sigStopHandler);
        signal(SIGQUIT, sigStopHandler);

        Server server(av[1], av[2]);
        server.run();
    }
    catch(const std::exception &e) {
        std::cerr << RED "!! ERROR !! " << e.what() << RESET << std::endl;
        return 1;
    }
    return 0;
}

