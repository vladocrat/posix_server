#include "tcpserver.h"

#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <vector>
#include <sys/epoll.h>
#include <fstream>
#include <functional>
#include <signal.h>
#include <time.h>

#include "ClientData.h"
#include "socket.h"
#include "utils.h"
#include "filecontroller.h"

namespace Internal
{
    void handleData(char* read, int s, int& command, ClientData& clientData, FileController& fc)
    {
        LOGM("handling data...");

        switch (command)
        {
        case 0:
        { // img
            LOGM("handling img data");
            clientData.imgDataLength = s;
            clientData.imgData = new char[clientData.imgDataLength];
            memcpy(clientData.imgData, read, s);
            command++;
            break;
        }
        case 1:
        { // machine name
            LOGM("handling machine name data");
            std::string str { "" };

            for (size_t i = 0; i < s; i++)
            {
                str += read[i];
            }

            clientData.machineName = str;
            command++;
            break;
        }
        case 2:
        { //timestamp
            LOGM("handling timestamp data");
            std::string str { "" };

            for (size_t i = 0; i < s; i++)
            {
                str += read[i];
            }

            clientData.timestamp = str;
            command++;

            if (!fc.save(clientData))
            {
                LOGE("failed to save client data");
            }

            LOGM("saved img");
            command = 0;

            break;
        }
        }
    }
} // Internal

struct TcpServer::impl_t
{
    FileController m_fileController;
    Socket m_connectionSocket;
    std::vector<Socket*> m_clients;
    uint32_t m_maxConnections { 10 };
    pthread_mutex_t clientsMutex;
    pthread_t m_thread;
};

TcpServer::TcpServer() noexcept
{
    createImpl();
}

TcpServer::~TcpServer() noexcept
{
    for (auto client : impl().m_clients)
    {
        delete client;
    }
}

bool TcpServer::run()
{
    if (!listen(3508, "127.0.0.1"))
    {
        LOGE("failed to listen on server");
        return false;
    }

    LOGM("server listening sucessfully");

    if (!start())
    {
        LOGE("failed to start server");
        return false;
    }

    return true;
}

bool TcpServer::listen(size_t port, const std::string& address)
{
    if (!impl().m_connectionSocket.valid())
    {
        LOGE("socket is not valid")
        return false;
    }

    Address addr(port, inet_addr(address.c_str()));
    impl().m_connectionSocket.setAddress(addr);

    if (!impl().m_connectionSocket.bind())
    {
        LOGE("faild to bind socket");
        LOGE(strerror(errno));
        return false;
    }

    LOGM("socket bound successfully");

    if (!impl().m_connectionSocket.listen(impl().m_maxConnections))
    {
        LOGE("socket failed to listen");
        return false;
    }

    return true;
}

bool TcpServer::start() noexcept
{
    std::cout << "This " << this << std::endl;

    int retVal = pthread_create(&impl().m_thread, NULL, threadFunc, this);

    if (retVal != 0)
    {
        LOGE("Error creating thread");
        return false;
    }

    return true;
}

void TcpServer::broadcast(union sigval sv) noexcept
{
    TcpServer* instance = reinterpret_cast<TcpServer*>(sv.sival_ptr);
    pthread_mutex_lock(&instance->impl().clientsMutex);

    for (auto& client : instance->impl().m_clients)
    {
        int32_t data = 1;

        if (!client->send(&data, sizeof(int32_t)))
        {
            LOGE("failed to send to client");
            LOGE(strerror(errno));
        }

        LOGM("succesfully sent");
    }

    pthread_mutex_unlock(&instance->impl().clientsMutex);
}

void TcpServer::trampoline(TcpServer *server)
{
    [[maybe_unused]] auto res = server->run();
}

void TcpServer::join()
{
    pthread_join(impl().m_thread, nullptr);
}

void* TcpServer::threadFunc(void *ctx)
{
    auto obj = reinterpret_cast<TcpServer*> (ctx);

    LOG("entering server thread");
    std::cout << "This " << ctx << std::endl;
    struct Epoll
    {
        Epoll(int _fd) : fd(_fd)
        {
        }

        ~Epoll()
        {
            if (!close(fd))
            {
                LOGE("failed to close epoll");
            }
        }

        bool valid()
        {
            return fd != -1;
        }

        int fd;
    };

    timer_t timerId;

    struct sigevent sev;
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = &TcpServer::broadcast;
    sev.sigev_value.sival_ptr = ctx;

    int result = timer_create(CLOCK_REALTIME, &sev, &timerId);

    if (result != 0)
    {
        std::cerr << "Failed to create timer." << std::endl;
    }

    struct itimerspec its;
    its.it_value.tv_sec = 2;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = 1;
    its.it_interval.tv_nsec = 0;

    result = timer_settime(timerId, 0, &its, nullptr);

    if (result != 0)
    {
        std::cerr << "Failed to arm timer." << std::endl;
    }

    Socket* client = new Socket;
    client->setDescriptor(-1);
    epoll_event event;
    epoll_event events[obj->impl().m_maxConnections];

    Epoll epoll(epoll_create1(0));

    if (!epoll.valid())
    {
        LOGE("failed to create epoll");
        return nullptr;
    }

    event.events = EPOLLIN | EPOLLHUP;
    event.data.fd = obj->impl().m_connectionSocket.descriptor();

    if (epoll_ctl(epoll.fd, EPOLL_CTL_ADD, obj->impl().m_connectionSocket.descriptor(), &event) == -1)
    {
        LOGE("failed to assign epoll");
        LOGE(strerror(errno));
        return nullptr;
    }

    while (true)
    {
        int ready { 0 };
        do {
            ready = epoll_wait(epoll.fd, events, obj->impl().m_maxConnections, 1000);
        }
        while (ready < 0 && errno == EINTR);

        if (ready == -1)
        {
            LOGE("failed to fetch any events");
            LOG(errno);
            continue;
        }

        for (size_t i = 0; i < ready; i++)
        {
            if (events[i].data.fd == obj->impl().m_connectionSocket.descriptor())
            {
                auto desc = obj->impl().m_connectionSocket.accept();
                client->setDescriptor(desc);

                if (!client->valid())
                {
                    LOGE(strerror(errno));
                }

                LOGM("client connected");
                event.events = EPOLLIN;
                event.data.fd = client->descriptor();

                if (epoll_ctl(epoll.fd, EPOLL_CTL_ADD, client->descriptor(), &event) < 0)
                {
                    LOGE("failed to ini epoll for client");
                }

                pthread_mutex_lock(&obj->impl().clientsMutex);
                obj->impl().m_clients.push_back(client);
                pthread_mutex_unlock(&obj->impl().clientsMutex);
                int32_t d = 1;

                if (!client->send(&d, sizeof(int32_t)))
                {
                    LOGE("failed initial handshake");
                }

                continue;
            }

            if (events[i].events & EPOLLHUP)
            {
                LOGM("client closed connection");
                auto it = std::find_if(obj->impl().m_clients.begin(), obj->impl().m_clients.end(), [&](Socket* socket) -> bool
                                       {
                                           return events[i].data.fd == socket->descriptor();
                                       });

                if (it != obj->impl().m_clients.end())
                {
                    obj->impl().m_clients.erase(it);
                    close(events[i].data.fd);
                    epoll_ctl(epoll.fd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                }
                else
                {
                    LOGE("no such client");
                }
            }

            Socket* socket { nullptr };
            for (auto client : obj->impl().m_clients)
            {
                if (client->descriptor() == events[i].data.fd)
                {
                    socket = client;
                }
            }

            if (!socket)
            {
                continue;
            }

            int s { -1 };
            auto read = socket->read(s);

            if (!read)
            {
                LOGM("didn't read anything");
                std::cout << obj->impl().m_clients.size() << std::endl;
                LOGM("client closed connection");
                auto it = std::find_if(obj->impl().m_clients.begin(), obj->impl().m_clients.end(), [&](Socket* socket) -> bool
                                       {
                                           return events[i].data.fd == socket->descriptor();
                                       });

                if (it != obj->impl().m_clients.end())
                {
                    obj->impl().m_clients.erase(it);
                    close(events[i].data.fd);
                    epoll_ctl(epoll.fd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                }
                else
                {
                    LOGE("no such client");
                }

                continue;
            }

            Internal::handleData(read, s, socket->itteration(), socket->data(), obj->impl().m_fileController);
        }
    }

    return 0;
}
