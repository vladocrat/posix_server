#pragma once

#include <string>

#include "pimpl.h"

class Parser final
{
public:
    enum class Signals
    {
        NONE,
        EXIT,
        ERROR,
        LS,
        CHECK
    };

    struct Res
    {
        Signals signal;
        std::string value;
    };

    Parser() noexcept;
    ~Parser() noexcept;

    Res parse(std::string& command) noexcept;

private:
    DECLARE_PIMPL;
};

