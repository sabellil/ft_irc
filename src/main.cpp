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


// static void printUserState(const User& user)
// {
//     std::cout << "nick       = [" << user.getNick() << "]" << std::endl;
//     std::cout << "username   = [" << user.getUsername() << "]" << std::endl;
//     std::cout << "realname   = [" << user.getRealname() << "]" << std::endl;
//     std::cout << "hasPass    = " << user.hasPass() << std::endl;
//     std::cout << "hasNick    = " << user.hasNick() << std::endl;
//     std::cout << "hasUser    = " << user.hasUser() << std::endl;
//     std::cout << "registered = " << user.isRegistered() << std::endl;
//     std::cout << "\n" << std::endl;
// }

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
