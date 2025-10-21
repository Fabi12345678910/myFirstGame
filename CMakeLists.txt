cmake_minimum_required(VERSION 3.23)

project(AdvancedCPP)

add_executable(server)

target_sources(server
  PRIVATE
    server.cpp
    Connection.cpp
    Events.cpp
    NetworkManager.cpp
)

add_executable(client)

target_sources(client
  PRIVATE
    client.cpp
    Connection.cpp
    Events.cpp
    NetworkManager.cpp
)
