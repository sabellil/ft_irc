#include "../include/Message.hpp"

Message::Message() : sender(0)
{
}

Message::~Message()
{
}

bool Message::parse(const std::string& line)
{
    clear();//TODO
    _rawLine = line;

    size_t i = 0;
    skipSpaces(line, i);//TODO

    if (i >= line.size())
        return false;

    size_t start = i;

    while (i < line.size() && line[i] != ' ')
        ++i;
    _command = line.substr(start, i - start);

    skipSpaces(line, i);

    while (i < line.size())
    {
        if (line[i] == ':')
        {
            _trailing = line.substr(i + 1);
            break;
        }
        start = i;
        while (i < line.size() && line[i] != ' ')
            ++i;

        if (start < i)
            _params.push_back(line.substr(start, i - start));

        skipSpaces(line, i);
    }
    return true;
}