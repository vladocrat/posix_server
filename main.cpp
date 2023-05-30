#include <iostream>

#include "tcpserver.h"
#include "parser.h"
#include "dir.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

namespace Internal
{
    void* consoleHandle(void*)
    {
        Parser parser;
        std::string input;

        while (true)
        {
            std::getline(std::cin, input);

            auto command = parser.parse(input);

            switch (command.signal)
            {
            case Parser::Signals::NONE:
                system(input.c_str());
                break;
            case Parser::Signals::ERROR:
                std::cerr << command.value << std::endl;
                break;
            case Parser::Signals::LS:
            {
                Dir dir("./");
                dir.print();
                break;
            }
            case Parser::Signals::CHECK:
            {
                std::string finalCommand { "xdg-open " };
                finalCommand += command.value;
                finalCommand += " &";
                system(finalCommand.c_str());
                break;
            }
            case Parser::Signals::EXIT:
                exit(0);
                break;
            default:
                std::cerr << "unknown command" << std::endl;
                break;
            }
        }
    }
} // Internal

int main()
{
    TcpServer server;
    TcpServer::trampoline(&server);

    pthread_t thread;
    auto code = pthread_create(&thread, nullptr, Internal::consoleHandle, nullptr);

    if (code != 0)
    {
        std::cerr << "failed to start console thread" << std::endl;
        return -1;
    }

    server.join();
    pthread_join(thread, nullptr);

    return 0;
}
