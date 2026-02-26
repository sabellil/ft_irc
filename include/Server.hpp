/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sabellil <sabellil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 17:25:06 by sabellil          #+#    #+#             */
/*   Updated: 2026/02/26 14:56:18 by sabellil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <vector>
#include <map>
#include <poll.h>

class User;
class Channel;
class Message;

class Server {
public:
    Server(int port, const std::string& password);
    ~Server();

    void run();

private:
    int         _port;
    std::string _password;
    bool        _running;

    int                 _serverFd;
    std::vector<pollfd> _pollFds;

    std::map<int, User*>               _usersByFd;
    // std::map<std::string, User*>      _usersByNick;
    std::map<std::string, Channel*>   _channels;
};