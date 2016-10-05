cmake_minimum_required(VERSION 3.5.1)
project(IRC-Chat-Room)
##
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


set(SERVER_FILES
        server.cpp
        TCP/TCPAcceptor.cpp
        TCP/TCPAcceptor.h
        TCP/TCPConnector.cpp
        TCP/TCPConnector.h
        TCP/TCPStream.cpp
        TCP/TCPStream.h
        Application/Channel.cpp Application/Channel.h
        Application/User.cpp Application/User.h
        Application/ClientQueue.cpp Application/ClientQueue.h
        Utilities.h)

set(CLIENT_FILES
        client.cpp
        TCP/TCPAcceptor.cpp
        TCP/TCPAcceptor.h
        TCP/TCPConnector.cpp
        TCP/TCPConnector.h
        TCP/TCPStream.cpp
        TCP/TCPStream.h
        Application/User.cpp Application/User.h
        Application/Channel.cpp Application/Channel.h
        Utilities.h)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

add_executable(server ${SERVER_FILES})
add_executable(client ${CLIENT_FILES})

set(THREAD_PREFER_PTHREAD_FLAG ON)
find_package (Threads)
target_link_libraries (server ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(client Threads::Threads)
