cmake_minimum_required(VERSION 3.23)

project(AdvancedCPP)

add_executable(server)

target_sources(server
  PRIVATE
    server.cpp
    ClientConnection.cpp
    ServerConnection.cpp
    Events.cpp
    NetworkManager.cpp
    ServerSocket.cpp
)

add_executable(client)

target_sources(client
  PRIVATE
    client.cpp
    ClientConnection.cpp
    ServerConnection.cpp
    Events.cpp
    NetworkManager.cpp
    ServerSocket.cpp
)
