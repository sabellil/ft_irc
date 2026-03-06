


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

//les tests du labo et des includes qui vont bien
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>

int main(int ac, char **av) {
    try {
        check_arg(ac, av);
        signal(SIGINT, sigStopHandler); // ctrl + c
        signal(SIGQUIT, sigStopHandler);// ctrl + backlash 

        int port = atoi(av[1]);
        std::string password = av[2];

        struct addrinfo hints; //le cahier des charges de l'adresse demandee
        struct addrinfo * result; //pointeur sur une liste chainee d'adresses possibles (svt on 8use la premiere)
        
        // init l'adresse a 0
        memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_socktype = 0;

        bool _run = getaddrinfo(NULL, av[1], &hints, &result); //pourquoi ca reprend une string ici et pas un int *suspicious*
        if (_run != false )
            std::cerr << "pas d'adresse correspondante dispo";

        //usage chat gpt pour main qui va afficher l'adresse (recopy)
        struct sockaddr_in * addr = (struct sockaddr_in*) result->ai_addr; 
        char ip_str[INET_ADDRSTRLEN];

        inet_ntop(AF_INET, &(addr->sin_addr), ip_str, sizeof(ip_str));

        std::cout << "IP : " << ip_str << std::endl; //0.0.0.0, normal on a pas encore le reste
        std::cout << "Port : " << ntohs(addr->sin_port) << std::endl;

        freeaddrinfo(result); // getaddrinfo alloue de la memoire ! ne pas oublier de la free
        

    }
    catch(const std::exception &e) {
        std::cerr << RED "!! ERROR !! " << e.what() << RESET << std::endl;
        return 1;
    }
    return 0;
}