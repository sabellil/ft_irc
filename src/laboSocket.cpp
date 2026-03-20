


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

        // int port = atoi(av[1]);
        std::string password = av[2];

        struct addrinfo hints; //le cahier des charges de l'adresse demandee
        struct addrinfo * result; //pointeur sur une liste chainee d'adresses possibles (svt on 8use la premiere)
        
        // init l'adresse a 0
        memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = 0;
        hints.ai_flags = AI_PASSIVE; // adresse passive = adresse en ecoute donc adresse serveur

        int status = getaddrinfo(NULL, av[1], &hints, &result); 
        //pourquoi ca reprend une string ici et pas un int *suspicious* TODO:investigate
        if (status != 0 )
            throw std::logic_error("No port available. Cannot launch server. ");

        int sockfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if (sockfd < 0 )
            throw std::logic_error("Fail socket. Cannot launch server. ");
        std::cout << "ok socket \n";

        if (bind(sockfd, result->ai_addr, result->ai_addrlen) < 0 )
            throw std::logic_error("Fail bind. Cannot launch server. ");
        std::cout << "ok bind \n";

        if (listen (sockfd, 10) < 0 )
            throw std::logic_error("deaf port. Cannot launch server. ");
        std::cout << "SERVER LISTENING \n";

        struct sockaddr_storage client_addr;
        socklen_t addr_size = sizeof(client_addr);

        int client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_size);

        if (client_fd < 0)
            throw std::logic_error("fail connexion client.. ");
        std::cout << "ok listen \n";

        std::cout << GREEN "\nClient connected !!" RESET << std::endl;

        freeaddrinfo(result); // getaddrinfo alloue de la memoire ! ne pas oublier de la free
        

    }
    catch(const std::exception &e) {
        std::cerr << RED "!! ERROR !! " << e.what() << RESET << std::endl;
        return 1;
    }
    return 0;
}

//./ircserv 6667 pass
//nc localhost 6667


//MAIN GPT POUR ACCEPTER PLUSIEURS CLIENTS MAIS JE VEUX COMPRENDRE
// while (true)
// {
//     struct sockaddr_storage client_addr;
//     socklen_t addr_size = sizeof(client_addr);

//     int client_fd = accept(sockfd,
//                            (struct sockaddr *)&client_addr,
//                            &addr_size);

//     if (client_fd < 0)
//     {
//         perror("accept");
//         continue;
//     }

//     std::cout << "New client connected\n";

//     send(client_fd, "Welcome to IRC\n", 15, 0);
// }