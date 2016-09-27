cmake_minimum_required(VERSION 3.6)
project(Project1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SERVER_FILES
    example_server.cpp
    TCPAcceptor.cpp
    TCPAcceptor.h
    TCPConnector.cpp
    TCPConnector.h
    TCPStream.cpp
    TCPStream.h)

set(CLIENT_FILES
        example_client.cpp
        TCPAcceptor.cpp
        TCPAcceptor.h
        TCPConnector.cpp
        TCPConnector.h
        TCPStream.cpp
        TCPStream.h)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

add_executable(server ${SERVER_FILES})
add_executable(client ${CLIENT_FILES})