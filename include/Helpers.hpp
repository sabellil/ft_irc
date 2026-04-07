#ifndef HELPERS_HPP
#define HELPERS_HPP

#include "Colors.hpp"
#include <csignal>
#include <cstdlib>
#include <cctype>
#include <iostream>
#include <stdexcept>

extern volatile sig_atomic_t g_run;

void    sigStopHandler(int signum);

void    check_arg(int ac, char **av);


#endif
