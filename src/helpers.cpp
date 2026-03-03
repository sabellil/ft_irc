#include "../include/Helpers.hpp"
#include <stdexcept>

int ft_atoi_port(const char* str)
{
    long res = 0;
    int i = 0;

    if (!str || !str[0])
        throw std::logic_error("Invalid port: empty\n");

    while (str[i])
    {
        if (str[i] < '0' || str[i] > '9')
            throw std::logic_error("Invalid port: digits only\n");

        res = res * 10 + (str[i] - '0');

        if (res > 65535)
            throw std::logic_error("Invalid port: too large\n");

        i++;
    }

    if (res < 1)
        throw std::logic_error("Invalid port: must be >= 1\n");

    return (int)res;
}