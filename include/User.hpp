/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sabellil <sabellil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 13:58:41 by sabellil          #+#    #+#             */
/*   Updated: 2026/02/27 11:59:16 by sabellil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>

class User
{
private:
    int                 _fd;

    std::string         _nick;
    std::string         _username;

    bool                _hasPass;
    bool                _hasNick;
    bool                _hasUser;
    bool                _registered;

    std::string         _inbuf;//s'assurer d'aller jusqu'au \r\n et d'avoir toute la ligne

public:
    User(int fd);
    ~User();

    std::string& inbuf() { return _inbuf; }//pour retourner le cumul des fragments de ligne recup par recv()
};