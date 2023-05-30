#pragma once

#include "pimpl.h"
#include "address.h"
#include "ClientData.h"

class Socket final
{
public:
    Socket() noexcept;
    ~Socket() noexcept;

    // public api
    [[nodiscard]] bool bind() noexcept;
    [[nodiscard]] bool bind(Address&) noexcept;
    [[nodiscard]] bool listen(size_t maxConnections) const noexcept;
    [[nodiscard]] int accept(sockaddr* addr = nullptr, socklen_t* len = nullptr) noexcept;

    [[nodiscard]] void* readAll(int flags = 0, bool* ok = nullptr) noexcept;
    [[nodiscard]] char* read(int&) noexcept;

    [[nodiscard]] bool send(const void* data, size_t size, int flags = 0) noexcept;

    [[nodiscard]] bool valid() const noexcept;
    [[nodiscard]] int32_t lastRead() const noexcept;
    [[nodiscard]] int32_t lastWritten() const noexcept;

    void setDescriptor(int desc) noexcept;
    int descriptor() const noexcept;
    void setAddress(const Address& addr) noexcept;
    Address address() const noexcept;
    ClientData& data() noexcept;
    int& itteration() noexcept;

private:
    DECLARE_PIMPL
};

