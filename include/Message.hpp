/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sabellil <sabellil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 17:25:03 by sabellil          #+#    #+#             */
/*   Updated: 2026/02/26 14:10:52 by sabellil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <vector>

class User; 

class Message{

public:
    std::string                     _raw;
    std::string                     _command;
    std::vector<std::string>        _params;
    std::string                     _trailing;
    User*                           sender;

    private:
    /*getters de commandes, params et trailing en const pour proteger d'une modif*/


}