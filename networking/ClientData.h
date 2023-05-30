#pragma once

#include <string>

struct ClientData
{
    int32_t nameLength;
    std::string machineName;
    int32_t imgDataLength;
    char* imgData;
    int32_t timestampLength;
    std::string timestamp;
};
