/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mairivie <mairivie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 16:51:38 by mairivie          #+#    #+#             */
/*   Updated: 2026/02/25 18:13:13 by mairivie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <csignal>
#include <iostream>
#include <string>
#include <exception>

void    check_arg(int ac, char **av) {
        if (ac != 3)
            throw std::logic_error("Wrong nbr of arguments. Use ./irc <port> <password>\n");
        for (int i=0; i<ac; i++) {
            std::cout << "av[" << i << "] : " << av[i] << std::endl;
        }
        //verif port, sinon trow
        //option check secure password
}

int main(int ac, char **av) {
    try {
        check_arg(ac, av);
        //init signal
        //init server
        //run server
        while(1){
            
        }
    }
    catch(const std::logic_error & error) {
        std::cout << "Caught: " << error.what();
    }
}
