cmake_minimum_required(VERSION 3.6)
project(Project1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")



set(SERVER_FILES
        server.cpp
        TCP/TCPAcceptor.cpp
        TCP/TCPAcceptor.h
        TCP/TCPConnector.cpp
        TCP/TCPConnector.h
        TCP/TCPStream.cpp
        TCP/TCPStream.h Application/User.cpp Application/User.h Application/ClientQueue.cpp Application/ClientQueue.h Application/Channel.cpp Application/Channel.h Utilities.h Application/Message.cpp Application/Message.h)

set(CLIENT_FILES
        example_client.cpp
        TCP/TCPAcceptor.cpp
        TCP/TCPAcceptor.h
        TCP/TCPConnector.cpp
        TCP/TCPConnector.h
        TCP/TCPStream.cpp
        TCP/TCPStream.h Application/User.cpp Application/User.h Application/Channel.cpp Application/Channel.h Utilities.h Application/Message.cpp Application/Message.h)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

add_executable(server ${SERVER_FILES})
add_executable(client ${CLIENT_FILES})

find_package (Threads)
target_link_libraries (server ${CMAKE_THREAD_LIBS_INIT})