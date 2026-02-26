/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sabellil <sabellil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 16:51:38 by mairivie          #+#    #+#             */
/*   Updated: 2026/02/25 17:32:22 by sabellil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int main(int ac, char **av) {
    // try {
        std::cout << av[0] << "Start of server and message class branch" << ac << std::endl;
        //check argc argv
        //init signal
        //init server
        //run server
    // }
    // catch {
    //     //error
    // }
}

/*
Boucle while (1)
{
    Lancement serveur si parsing initial OK
    if (peut lire sur la socket) POLLIN
        read();
    if (peut ecrire sur la socket) POLLOUT
        write();
}
*/

/*
TODO: 

- Ecrire classe Serveur
- Ecrire boucle main de sondage du serveur
    - timeout ?
- Lister commandes client et interpretation IRC
- Ecrire classe Message
*/