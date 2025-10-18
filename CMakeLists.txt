cmake_minimum_required(VERSION 3.10)
project(MyGame)

set(CMAKE_CXX_STANDARD 17)

find_package(SFML 2.6 COMPONENTS system window graphics audio REQUIRED)

include_directories(include)

add_executable(MyGame
    src/main.cpp
    src/Game.cpp
    src/Player.cpp
    src/stage/Floor.cpp
)

target_link_libraries(MyGame sfml-system sfml-window sfml-graphics sfml-audio)
