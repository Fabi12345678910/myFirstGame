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

add_executable(eventTest)

target_sources(eventTest 
  PRIVATE
    src/eventTest.cpp
    src/networking/impl/Events.cpp)

target_compile_features(eventTest PRIVATE cxx_std_17)
target_link_libraries(eventTest PRIVATE SFML::Network)
