#pragma once

#include <iostream>
#include <atomic>

namespace Utils
{

inline void printArr(char* arr, int32_t len);

inline uintmax_t currentThreadId();

} // Utils

#define DEBUG
#ifdef DEBUG
    #define LOGE(x) \
        { \
        std::string error = "[ERROR]: "; \
        error += x; \
        std::cerr << error << std::endl; \
        }

    #define LOGM(x) \
        { \
        std::string msg = "[MSG]: "; \
        msg += x; \
        std::cout << msg << std::endl; \
        }

    #define LOGARR(arr, len) Utils::printArr(arr, len);
#else
    #define LOGE(x)
    #define LOGM(x)
    #define LOGARR(arr, len)
#endif

#define LOG(x) std::cout << x << std::endl;



namespace Utils
{

inline void printArr(char* arr, int32_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        std::cout << arr[i];
    }
    std::cout << std::endl;
}

inline uintmax_t currentThreadId()
{
    static std::atomic_uintmax_t counter;
    thread_local uintmax_t threadId = counter.fetch_add((uintmax_t)1);
    return threadId;
}

} // Utils
