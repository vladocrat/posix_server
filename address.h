#pragma once

#include <netinet/in.h>

#include "pimpl.h"

class Address final
{
public:
    Address() noexcept;
    Address(in_port_t port, in_addr_t address) noexcept;
    Address(const Address& other) noexcept;
    ~Address() noexcept;

    void setFamily(sa_family_t family);
    void setPort(in_port_t port);
    void setAddr(in_addr address);

    sockaddr_in* addr() noexcept;
    const sockaddr_in* addr() const noexcept;

private:
    DECLARE_PIMPL
};
