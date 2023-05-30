cmake_minimum_required(VERSION 3.5)

project(posix_server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(posix_server
    main.cpp
    socket.h socket.cpp
    dir.h dir.cpp
    pimpl.h
    tcpserver.h tcpserver.cpp
    Singleton.h
    ClientData.h
    filecontroller.h filecontroller.cpp
    parser.h parser.cpp
    utils.h
    address.h address.cpp)

install(TARGETS posix_server
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
