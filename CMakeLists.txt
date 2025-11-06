cmake_minimum_required(VERSION 3.28)
project(AdvancedCPP LANGUAGES CXX)

# Global language standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Speed up + avoid building SFML extras
include(FetchContent)
# Turn off SFML extras when fetched
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_TEST_SUITE OFF CACHE BOOL "" FORCE)
set(SFML_USE_SYSTEM_DEPS OFF CACHE BOOL "" FORCE)
# Build shared libs so we don't fight static CRT on MSVC
set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)

FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)
FetchContent_MakeAvailable(SFML)

include_directories(include)

# Helper function: auto-copy dependent DLLs beside the exe (Windows)
function(enable_autocopy_runtime_dlls targetName)
    if (WIN32)
        add_custom_command(TARGET ${targetName} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_RUNTIME_DLLS:${targetName}> $<TARGET_FILE_DIR:${targetName}>
            COMMAND_EXPAND_LISTS
        )
    endif()
endfunction()

# ---- Executables -------------------------------------------------------------

add_executable(eventTest
    src/eventTest.cpp
    src/networking/Events.cpp
)
target_link_libraries(eventTest PRIVATE SFML::Network)
enable_autocopy_runtime_dlls(eventTest)

add_executable(ConnectionTestClient
    src/tests/ConnectionTestClient.cpp
    src/networking/Events.cpp
    src/networking/Connection.cpp
)
target_link_libraries(ConnectionTestClient PRIVATE SFML::Network)
enable_autocopy_runtime_dlls(ConnectionTestClient)

add_executable(ConnectionTestServer
    src/tests/ConnectionTestServer.cpp
    src/networking/Events.cpp
    src/networking/Connection.cpp
    src/networking/ServerSocket.cpp
)
target_link_libraries(ConnectionTestServer PRIVATE SFML::Network)
enable_autocopy_runtime_dlls(ConnectionTestServer)

add_executable(MyGameLocal
    src/main.cpp
    src/Game.cpp
)
target_link_libraries(MyGameLocal PRIVATE
    SFML::Network SFML::System SFML::Window SFML::Graphics SFML::Audio
)
enable_autocopy_runtime_dlls(MyGameLocal)

add_executable(Server
    src/Server.cpp
    src/operations/GameUpdate.cpp
    src/operations/PlayerOperations.cpp
    src/networking/Events.cpp
    src/rendering/Renderer.cpp
    src/networking/Connection.cpp
    src/networking/ServerSocket.cpp
)
target_link_libraries(Server PRIVATE
    SFML::Network SFML::System SFML::Window SFML::Graphics SFML::Audio
)
enable_autocopy_runtime_dlls(Server)

add_executable(Client
    src/Client.cpp
    src/operations/GameUpdate.cpp
    src/operations/PlayerOperations.cpp
    src/rendering/Renderer.cpp
    src/networking/Events.cpp
    src/networking/Connection.cpp
    src/networking/ServerSocket.cpp
)
target_link_libraries(Client PRIVATE
    SFML::Network SFML::System SFML::Window SFML::Graphics SFML::Audio
)
enable_autocopy_runtime_dlls(Client)

add_executable(TestLogin
    src/tests/TestLogin.cpp
    src/networking/Events.cpp
    src/networking/Connection.cpp
    src/networking/ServerSocket.cpp
)
target_link_libraries(TestLogin PRIVATE
    SFML::Network SFML::System SFML::Window SFML::Graphics SFML::Audio
)
enable_autocopy_runtime_dlls(TestLogin)

# (Optional) warnings
if (MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Make running from VS/VSCode look at repo root for assets
set_property(TARGET MyGameLocal Client Server
    PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)
