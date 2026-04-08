#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <string>
#include <vector>
#include <iostream>

class User; 

class Message{

public:

    Message();
    ~Message();

    std::string                     _raw;
    std::string                     _command;
    std::vector<std::string>        _params;
    std::string                     _trailing;

    bool                            parse(const std::string& line);
    void                            skipSpaces(const std::string& line, size_t& i);
    void                            clear();
};

#endif