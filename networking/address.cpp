#include "address.h"

struct Address::impl_t
{
    sockaddr_in m_address;
};

Address::Address() noexcept
{
    createImpl();
}

Address::Address(in_port_t port, in_addr_t address) noexcept
{
    createImpl();

    impl().m_address.sin_family = AF_INET;
    impl().m_address.sin_port = htons(port);
    impl().m_address.sin_addr.s_addr = address;
}

Address::Address(const Address& other) noexcept
{
    createImpl();

    impl().m_address.sin_family = (other.addr()->sin_family);
    impl().m_address.sin_addr = (other.addr()->sin_addr);
    impl().m_address.sin_port = (other.addr()->sin_port);
}

Address::~Address() noexcept
{

}

void Address::setFamily(sa_family_t family)
{
    impl().m_address.sin_family = family;
}

void Address::setPort(in_port_t port)
{
    impl().m_address.sin_port = port;
}

void Address::setAddr(in_addr address)
{
    impl().m_address.sin_addr = address;
}

const sockaddr_in* Address::addr() const noexcept
{
    return &impl().m_address;
}

sockaddr_in* Address::addr() noexcept
{
    return &impl().m_address;
}

