# MyFirstGame
MyFirstGame is a fast paced multiplayer only pvp shooter

While jumping around and shooting is definitely possiple solo, actual gameplay requires a second person for fun or a second client.
# Requirements
## general
- cmake v3.28 or higher
- c++17 compiler
- window libraries(more details in the linux section)
## Windows
- Microsoft C++ Build Tools
## Linux
- cmake, gcc, g++

Additional packages(as used in the ci on debian trixie):
- libx11-dev libxrandr-dev libxcursor-dev libxinerama-dev libxi-dev
- libglvnd-dev libglu1-mesa-dev libglx-dev
- libudev-dev
# Compilation
1. `cmake -B build`
2. `cd build && cmake --build .`