#ifndef HELPERS_HPP
#define HELPERS_HPP

#include "colors.hpp"
#include <csignal>
#include <cstdlib>
#include <cctype>
#include <iostream>
#include <stdexcept>

void    sigStopHandler(int signum);

void    check_arg(int ac, char **av);


// int     ft_atoi_port(const char* str);

#endif