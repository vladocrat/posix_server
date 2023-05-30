cmake_minimum_required(VERSION 3.5)

project(posix_server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(UTILS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/utils)
set(NETWORKING_PATH ${CMAKE_CURRENT_SOURCE_DIR}/networking)
set(FILESYSTEM_PATH ${CMAKE_CURRENT_SOURCE_DIR}/filesystem)

add_executable(posix_server
    main.cpp
    ${NETWORKING_PATH}/socket.h ${NETWORKING_PATH}/socket.cpp
    ${NETWORKING_PATH}/tcpserver.h ${NETWORKING_PATH}/tcpserver.cpp
    ${NETWORKING_PATH}/address.h ${NETWORKING_PATH}/address.cpp
    ${NETWORKING_PATH}/ClientData.h

    ${UTILS_PATH}/utils.h
    ${UTILS_PATH}/pimpl.h
    ${UTILS_PATH}/Singleton.h

    ${FILESYSTEM_PATH}/filecontroller.h ${FILESYSTEM_PATH}/filecontroller.cpp
    ${FILESYSTEM_PATH}/parser.h ${FILESYSTEM_PATH}/parser.cpp
    ${FILESYSTEM_PATH}/dir.h ${FILESYSTEM_PATH}/dir.cpp
    )

include_directories(utils)
include_directories(networking)
include_directories(filesystem)

add_definitions(-D_FILE_OFFSET_BITS=64)

install(TARGETS posix_server
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
