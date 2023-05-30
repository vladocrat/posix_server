#pragma once

#include "pimpl.h"
#include "Singleton.h"

class TcpServer final : public Singleton<TcpServer>
{
public:
    TcpServer() noexcept;
    ~TcpServer() noexcept;

    [[nodiscard]] bool run();
    [[nodiscard]] bool listen(size_t port, const std::string& address);
    [[nodiscard]] bool start() noexcept;
    static void broadcast(union sigval sv) noexcept;
    static void trampoline(TcpServer* server);
    void join();

private:
    DECLARE_PIMPL

    static void* threadFunc(void* ctx);
};

