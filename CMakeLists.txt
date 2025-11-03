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
    src/networking/Events.cpp)

target_compile_features(eventTest PRIVATE cxx_std_17)
target_link_libraries(eventTest PRIVATE SFML::Network)

add_executable(ConnectionTestClient)

target_sources(ConnectionTestClient
  PRIVATE
    src/tests/ConnectionTestClient.cpp
    src/networking/Events.cpp
    src/networking/Connection.cpp)

target_compile_features(ConnectionTestClient PRIVATE cxx_std_17)
target_link_libraries(ConnectionTestClient PRIVATE SFML::Network)


add_executable(ConnectionTestServer)

target_sources(ConnectionTestServer
  PRIVATE
    src/tests/ConnectionTestServer.cpp
    src/networking/Events.cpp
    src/networking/Connection.cpp
    src/networking/ServerSocket.cpp)

target_compile_features(ConnectionTestServer PRIVATE cxx_std_17)
target_link_libraries(ConnectionTestServer PRIVATE SFML::Network)

add_executable(MyGameLocal
    src/main.cpp
    src/Game.cpp
)
target_compile_features(MyGameLocal PRIVATE cxx_std_17)
target_link_libraries(MyGameLocal PRIVATE SFML::Network SFML::System SFML::Window SFML::Graphics SFML::Audio)

add_executable(Server
    src/Server.cpp
    src/operations/GameUpdate.cpp
    src/operations/PlayerOperations.cpp
    src/networking/Events.cpp
    src/rendering/Renderer.cpp
    src/networking/Connection.cpp
    src/networking/ServerSocket.cpp
)
target_compile_features(Server PRIVATE cxx_std_17)
target_link_libraries(Server PRIVATE SFML::Network SFML::System SFML::Window SFML::Graphics SFML::Audio)

add_executable(Client
    src/Client.cpp
    src/operations/GameUpdate.cpp
    src/operations/PlayerOperations.cpp
    src/rendering/Renderer.cpp
    src/networking/Events.cpp
    src/networking/Connection.cpp
    src/networking/ServerSocket.cpp
)
target_compile_features(Client PRIVATE cxx_std_17)
target_link_libraries(Client PRIVATE SFML::Network SFML::System SFML::Window SFML::Graphics SFML::Audio)

add_executable(TestLogin
    src/tests/TestLogin.cpp
    src/networking/Events.cpp
    src/networking/Connection.cpp
    src/networking/ServerSocket.cpp
)
target_compile_features(TestLogin PRIVATE cxx_std_17)
target_link_libraries(TestLogin PRIVATE SFML::Network SFML::System SFML::Window SFML::Graphics SFML::Audio)
