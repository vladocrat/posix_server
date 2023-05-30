#include "parser.h"

#include <vector>

namespace Internal
{
std::vector<std::string> split(std::string& str)
{
    std::string delimiter = " ";
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = str.find(delimiter, pos_start)) != std::string::npos)
    {
        token = str.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back(str.substr(pos_start));
    return res;
}
} // Internal

struct Parser::impl_t
{

};

Parser::Parser() noexcept
{
    createImpl();
}

Parser::~Parser() noexcept
{

}

Parser::Res Parser::parse(std::string& command) noexcept
{
    auto args = Internal::split(command);

    if (args[0] == "exit")
    {
        return Res {Signals::EXIT, "closing server"};
    }
    else if (args[0] == "ls")
    { // list all dirs registered on server
        return Res {Signals::LS};
    }
    else if (args[0] == "check")
    {
        if (args.size() > 2)
        {
            return Res {Signals::ERROR, "too many arguments"};
        }

        return Res {Signals::CHECK, args[1]};
    }
    else
    {
        return Res {Signals::NONE};
    }

    return Res {Signals::NONE};
}
