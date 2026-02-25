/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mairivie <mairivie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 16:51:38 by mairivie          #+#    #+#             */
/*   Updated: 2026/02/25 19:31:36 by mairivie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <csignal>
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

void    sigStopHandler(int signum) {
    std::cout << "Interruption du serveur par signal " << signum << std::endl;
    std::exit(signum); // exit pas dans les fonctions allowed, TODO: une sorte de ft clean close
}

void    check_arg(int ac, char **av) {
        if (ac != 3)
            throw std::logic_error("Wrong nbr of arguments. Use ./irc <port> <password>\n");
        for (int i=0; i<ac; i++) {
            std::cout << "av[" << i << "] : " << av[i] << std::endl;
        }
        //TODO: 
        //pour verif port, est-ce qu'une fonction existe pour test ?
        //option check secure password (bif bof la motiv)
}

int main(int ac, char **av) {
    try {
        check_arg(ac, av);
        signal(SIGINT, *sigStopHandler); // ctrl + c
        signal(SIGQUIT, *sigStopHandler); //ctrl + \
        //init server
        //run server
        while(1){
            
        }
    }
    catch(const std::logic_error & error) {
        std::cout << "Caught: " << error.what();
    }
}
