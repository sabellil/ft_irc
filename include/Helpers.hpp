#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <csignal>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <cctype>

void    sigStopHandler(int signum);

void    check_arg(int ac, char **av);


// int     ft_atoi_port(const char* str);

#endif