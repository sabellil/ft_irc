#include "../include/Message.hpp"
#include <cctype>
Message::Message() : sender(NULL)
{
}

Message::~Message()
{
}

void Message::clear()
{
    _raw.clear();
    _command.clear();
    _params.clear();
    _trailing.clear();
}

void Message::skipSpaces(const std::string& line, size_t& i)
{
    while (i < line.size() && line[i] == ' ')
        ++i;
}

bool Message::parse(const std::string& line)
{
    clear();
    _raw = line;

    size_t i = 0;
    skipSpaces(line, i);

    if (i >= line.size())
        return false;

    size_t start = i;
    while (i < line.size() && line[i] != ' ')
        ++i;

    _command = line.substr(start, i - start);
    for (size_t j = 0; j < _command.size(); ++j)
        _command[j] = std::toupper(static_cast<unsigned char>(_command[j]));
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