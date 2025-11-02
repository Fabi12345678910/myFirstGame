cmake_minimum_required(VERSION 3.28)
set(CMAKE_CXX_STANDARD 17)

project(AdvancedCPP)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

include_directories(include)

add_executable(eventTest)

target_sources(eventTest
  PRIVATE
    src/eventTest.cpp
    src/networking/impl/Events.cpp)

target_compile_features(eventTest PRIVATE cxx_std_17)
target_link_libraries(eventTest PRIVATE SFML::Network)

add_executable(connectionTestClient)

target_sources(connectionTestClient
  PRIVATE
    src/tests/connectionTestClient.cpp
    src/networking/impl/Events.cpp
    src/networking/impl/Connection.cpp)

target_compile_features(connectionTestClient PRIVATE cxx_std_17)
target_link_libraries(connectionTestClient PRIVATE SFML::Network)


add_executable(connectionTestServer)

target_sources(connectionTestServer
  PRIVATE
    src/tests/connectionTestServer.cpp
    src/networking/impl/Events.cpp
    src/networking/impl/Connection.cpp
    src/networking/impl/ServerSocket.cpp)

target_compile_features(connectionTestServer PRIVATE cxx_std_17)
target_link_libraries(connectionTestServer PRIVATE SFML::Network)

add_executable(MyGameLocal
    src/main.cpp
    src/Game.cpp
)
target_compile_features(MyGameLocal PRIVATE cxx_std_17)
target_link_libraries(MyGameLocal PRIVATE SFML::Network SFML::System SFML::Window SFML::Graphics SFML::Audio)

add_executable(Server
    src/Server.cpp
    src/operations/GameUpdate.cpp
    src/networking/impl/Events.cpp
    src/networking/impl/Connection.cpp
    src/networking/impl/ServerSocket.cpp
)
target_compile_features(Server PRIVATE cxx_std_17)
target_link_libraries(Server PRIVATE SFML::Network SFML::System SFML::Window SFML::Graphics SFML::Audio)


add_executable(testLogin
    src/tests/testLogin.cpp
    src/networking/impl/Events.cpp
    src/networking/impl/Connection.cpp
    src/networking/impl/ServerSocket.cpp
)
target_compile_features(testLogin PRIVATE cxx_std_17)
target_link_libraries(testLogin PRIVATE SFML::Network SFML::System SFML::Window SFML::Graphics SFML::Audio)
