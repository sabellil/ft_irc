#include "../include/Message.hpp"

Message::Message() : sender(0)
{
}

Message::~Message()
{
}


bool Message::parse(const std::string& line)
{
    clear();//TODO : vider tous les elements de Message pour repartir de 0
    _rawLine = line;

    size_t i = 0;
    skipSpaces(line, i);//TODO, jump les eventuels espaces au debut du msg

    if (i >= line.size())
        return false;

    size_t start = i;//command
    while (i < line.size() && line[i] != ' ')
        ++i;
    _command = line.substr(start, i - start);
    skipSpaces(line, i);

    while (i < line.size())//trailing
    {
        if (line[i] == ":")
        {
            _trailing = line.substr(i + 1);
        }
    }

    //params a recuperer ici 



}