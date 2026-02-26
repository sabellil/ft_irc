/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sabellil <sabellil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 13:58:41 by sabellil          #+#    #+#             */
/*   Updated: 2026/02/26 14:48:32 by sabellil         ###   ########.fr       */
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

public:
    User(int fd);
    ~User();
};