#include "socket.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <string.h>

#include "utils.h"

namespace Internal
{
    int32_t readSize(int desc)
    {
        int32_t ret;
        auto data = (char*)&ret;
        int left = sizeof(ret);
        int rc { -1 };

        do {
            rc = ::read(desc, data, left);
            if (rc <= 0) {
                if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                    // use select() or epoll() to wait for the socket to be readable again
                }
                else if (errno != EINTR) {
                    return -1;
                }
            }
            else {
                data += rc;
                left -= rc;
            }
        } while (left > 0);

        return ret;
    }

    char* readData(int desc, int size)
    {
        auto data = new char[size];
        int rc = 0;

        do {
            int needed = size - rc;
            char* target = data + rc;
            rc += ::read(desc, target, needed);

            if (rc <= 0) {
                if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                    // use select() or epoll() to wait for the socket to be readable again
                }
                else if (errno != EINTR) {
                    return nullptr;
                }
            }
        } while ((rc - size) != 0);

        std::cout << "read rc: " << rc << std::endl;

        return data;
    }
} // Internal

struct Socket::impl_t
{
    int m_socket { -1 };
    Address m_address;
    int32_t m_lastRead { -1 };
    int32_t m_lastWritten { -1 };
    int32_t m_packetSize { 0 };
    ClientData m_data;
    int m_itteration { 0 };
};

Socket::Socket() noexcept
{
    createImpl();

    impl().m_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
}

Socket::~Socket() noexcept
{
    close(impl().m_socket);
}

bool Socket::bind() noexcept
{
    return bind(impl().m_address);
}

bool Socket::bind(Address& address) noexcept
{
    return ::bind(impl().m_socket,
                  reinterpret_cast<sockaddr*>(impl().m_address.addr()),
                  sizeof (*impl().m_address.addr())) != -1;
}

bool Socket::listen(size_t maxConnections) const noexcept
{
    return ::listen(impl().m_socket, maxConnections) == 0;
}

int Socket::accept(sockaddr* addr, socklen_t* len) noexcept
{
    return ::accept(impl().m_socket, addr, len);
}

void* Socket::readAll(int flags, bool* ok) noexcept
{
    void* buffer;
    auto read = recv(impl().m_socket, buffer, 100, flags);
    impl().m_lastRead = read;

    if (!ok)
    {
        return buffer;
    }

    if (read == -1)
    {
        *ok = false;
    }

    return buffer;
}

char* Socket::read(int& r) noexcept
{
    auto size = Internal::readSize(impl().m_socket);
    LOGM("read size");
    LOG("size: " << size);

    if (size < 0)
    {
        return nullptr;
    }

    auto read = Internal::readData(impl().m_socket, size);
    LOGM("finished reading");
    r = size;
    return read;
}

bool Socket::send(const void* data, size_t size, int flags) noexcept
{
    auto sent = ::send(impl().m_socket, data, size, flags);
    impl().m_lastWritten = sent;

    return sent != -1;
}

bool Socket::valid() const noexcept
{
    return impl().m_socket != -1;
}

int32_t Socket::lastRead() const noexcept
{
    return impl().m_lastRead;
}

int32_t Socket::lastWritten() const noexcept
{
    return impl().m_lastWritten;
}

void Socket::setDescriptor(int desc) noexcept
{
    impl().m_socket = desc;
}

int Socket::descriptor() const noexcept
{
    return impl().m_socket;
}

void Socket::setAddress(const Address& other) noexcept
{
    impl().m_address.setFamily(other.addr()->sin_family);
    impl().m_address.setAddr(other.addr()->sin_addr);
    impl().m_address.setPort(other.addr()->sin_port);
}

Address Socket::address() const noexcept
{
    return impl().m_address;
}

ClientData& Socket::data() noexcept
{
    return impl().m_data;
}

int& Socket::itteration() noexcept
{
    return impl().m_itteration;
}
